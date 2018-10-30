#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <stdlib.h>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::string;
using std::to_string;
using std::vector;
using std::cerr;
using std::endl;

Emitter::Emitter(Compiler &c) noexcept
: rt(c.getRT()),
  dl(c.getDataLayout()),
  ctx(c.getContext()),
  ir(ctx),
  types(ctx, dl)
{
}

RTModule *
Emitter::createModule(std::string const &name)
{
    auto *module = new llvm::Module(name, ctx);

    vector<llvm::Type *> argTypes;

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            llvm::Type::getVoidTy(ctx),
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    "__body__",
                    module);

    llvm::BasicBlock::Create(ctx, "", func);

    return new RTModule(name, module, types, func);
}

llvm::Value *
Emitter::emit(RTModule &mod, AST const &ast)
{
    switch (ast.getType()) {
    case ASTType::ExprIntLit: return emit(mod, cast<IntLitExpr>(ast));

    case ASTType::ExprIdent: {
        auto &expr = cast<IdentExpr>(ast);

        if (expr.name == "None") {
            return ir.CreateCall(mod.llvmPy_none(), {});
        }

        auto *slot = mod.getScope().slots[expr.name];

        if (slot == nullptr) {
            throw "Not Implemented";
        } else {
            return ir.CreateLoad(slot);
        }
    }

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = mod.getScope().slots[ident];
        auto *rhs = emit(mod, stmt.rhs);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(mod, expr.expr);
    }

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(mod, expr.lhs);
        auto *rhs = emit(mod, expr.rhs);

        switch (expr.op) {
        case tok_add: return ir.CreateCall(mod.llvmPy_add(), { lhs, rhs });
        default: return nullptr;
        }
    }

    case ASTType::ExprCall: return emit(mod, cast<CallExpr>(ast));

    case ASTType::ExprLambda: {
        auto &lambda = cast<LambdaExpr>(ast);
        auto *func = emitFunc(
                "lambda",
                mod,
                { new ReturnStmt(lambda.expr) });
        return ir.CreateCall(mod.llvmPy_func(), { &func->getFunction() });
    }

    default:
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTModule &mod, CallExpr const &call)
{
    llvm::Value *lhs = emit(mod, call.lhs);
    vector<llvm::Value *> args;

    for (auto *arg : call.args) {
        args.push_back(emit(mod, *arg));
    }

    llvm::Value *np = llvm::ConstantInt::get(types.PyIntValue, args.size());
    llvm::CallInst *inst = ir.CreateCall(mod.llvmPy_fchk(), {lhs, np});

    if (args.size() == 0) {
        return ir.CreateCall(inst, {});
    } else if (args.size() == 1) {
        return ir.CreateCall(inst, {args[0]});
    } else {
        cerr << "Cannot call function with more than 1 arguments ("
             << args.size() << " provided)" << endl;
        exit(1);
    }
}

llvm::Value *
Emitter::emit(RTModule &mod, IntLitExpr const &expr)
{
    llvm::ConstantInt *value =
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    static_cast<uint64_t>(expr.value));

    return ir.CreateCall(mod.llvmPy_int(), {value});
}

llvm::Value *
Emitter::emit(RTModule &mod, std::vector<Stmt *> const &stmts)
{
    llvm::Value *lastValue = nullptr;

    // Store original insert point.
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();

    // Begin inserting into module body.
    llvm::BasicBlock *init = &mod.getFunction().getEntryBlock();
    llvm::BasicBlock *prog = &mod.getFunction().back();

    for (auto *stmt : stmts) {
        if (stmt->getType() == ASTType::StmtAssign) {
            ir.SetInsertPoint(init);
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            auto *alloca = ir.CreateAlloca(types.Ptr);
            mod.getScope().slots[ident] = alloca;

            ir.CreateStore(
                    llvm::ConstantPointerNull::get(alloca->getType()),
                    alloca);
        }
    }

    for (auto *stmt : stmts) {
        ir.SetInsertPoint(prog);
        lastValue = emit(mod, *stmt);
    }

    // Restore original insert point.
    ir.SetInsertPoint(insertPoint);

    return lastValue;
}

RTFunc *
Emitter::emitFunc(
        std::string const &name,
        RTModule &mod,
        std::vector<Stmt *> const &stmts)
{
    RTScope *scope = new RTScope(mod.getScope());
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();
    vector<llvm::Type *> argTypes;

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            types.Ptr,
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    name,
                    &mod.getModule());

    llvm::BasicBlock *prog = llvm::BasicBlock::Create(ctx, "", func);
    bool lastIsRet = false;

    for (auto *stmt : stmts) {
        ir.SetInsertPoint(prog);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            llvm::Value *value = emit(mod, ret.expr);
            ir.CreateRet(value);
            lastIsRet = true;
        } else {
            emit(mod, *stmt);
            lastIsRet = false;
        }
    }

    if (!lastIsRet) {
        // Return None if no explicit return.
        ir.CreateRet(llvm::ConstantPointerNull::get(types.Ptr));
    }

    llvm::verifyFunction(*func);
    ir.SetInsertPoint(insertPoint);

    RTFunc *rtFunc = new RTFunc(func, scope);

    func->setPrefixData(
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    (uint64_t) rtFunc));

    return rtFunc;
}
