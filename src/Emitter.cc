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

static struct {
    string FuncObj = "fo";
    string PosArgCount = "np";
    string FuncPtr = "fp";
    string RetVal = "rv";
    string Arg = "a";
} tags;

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
Emitter::emit(RTScope &scope, AST const &ast)
{
    RTModule &mod = scope.getModule();

    switch (ast.getType()) {
    case ASTType::ExprIntLit: return emit(scope, cast<IntLitExpr>(ast));

    case ASTType::ExprIdent: {
        auto &expr = cast<IdentExpr>(ast);

        if (expr.name == "None") {
            return ir.CreateCall(mod.llvmPy_none(), {});
        }

        auto *slot = scope.slots[expr.name];

        if (slot == nullptr) {
            throw "Not Implemented";
        } else {
            return ir.CreateLoad(slot);
        }
    }

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = scope.slots[ident];
        auto *rhs = emit(scope, stmt.rhs);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(scope, expr.expr);
    }

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(scope, expr.lhs);
        auto *rhs = emit(scope, expr.rhs);

        switch (expr.op) {
        case tok_add: return ir.CreateCall(mod.llvmPy_add(), { lhs, rhs });
        default: return nullptr;
        }
    }

    case ASTType::ExprCall: return emit(scope, cast<CallExpr>(ast));

    case ASTType::ExprLambda: {
        auto &lambda = cast<LambdaExpr>(ast);
        auto *func = emitFunc(
                "lambda",
                scope,
                { new ReturnStmt(lambda.expr) });
        return ir.CreateCall(mod.llvmPy_func(), { &func->getFunction() });
    }

    default:
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, CallExpr const &call)
{
    RTModule &mod = scope.getModule();

    llvm::Value *lhs = emit(scope, call.lhs);
    lhs->setName(tags.FuncObj);
    vector<llvm::Value *> args;

    for (auto *arg : call.args) {
        llvm::Value *value = emit(scope, *arg);
        value->setName(tags.Arg);
        args.push_back(value);
    }

    // Count of positional arguments.
    llvm::Value *np = llvm::ConstantInt::get(types.PyIntValue, args.size());

    llvm::CallInst *inst = ir.CreateCall(
            mod.llvmPy_fchk(),
            {lhs, np},
            tags.FuncPtr);

    if (args.size() == 0) {
        return ir.CreateCall(inst, {}, tags.RetVal);
    } else if (args.size() == 1) {
        return ir.CreateCall(inst, {args[0]}, tags.RetVal);
    } else {
        cerr << "Cannot call function with more than 1 arguments ("
             << args.size() << " provided)" << endl;
        exit(1);
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, IntLitExpr const &expr)
{
    RTModule &mod = scope.getModule();

    llvm::ConstantInt *value =
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    static_cast<uint64_t>(expr.value));

    return ir.CreateCall(mod.llvmPy_int(), {value});
}

llvm::Value *
Emitter::emit(RTScope &scope, std::vector<Stmt *> const &stmts)
{
    RTModule &mod = scope.getModule();

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
        lastValue = emit(scope, *stmt);
    }

    // Restore original insert point.
    ir.SetInsertPoint(insertPoint);

    return lastValue;
}

RTFunc *
Emitter::emitFunc(
        std::string const &name,
        RTScope &outerScope,
        std::vector<Stmt *> const &stmts)
{
    RTModule &mod = outerScope.getModule();

    RTScope *innerScope = outerScope.createDerived();
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
            llvm::Value *value = emit(*innerScope, ret.expr);
            ir.CreateRet(value);
            lastIsRet = true;
        } else {
            emit(*innerScope, *stmt);
            lastIsRet = false;
        }
    }

    if (!lastIsRet) {
        // Return None if no explicit return.
        ir.CreateRet(llvm::ConstantPointerNull::get(types.Ptr));
    }

    llvm::verifyFunction(*func);
    ir.SetInsertPoint(insertPoint);

    RTFunc *rtFunc = new RTFunc(*func, *innerScope);

    func->setPrefixData(
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    (uint64_t) rtFunc));

    return rtFunc;
}
