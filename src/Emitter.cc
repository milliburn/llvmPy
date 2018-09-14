#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::string;
using std::to_string;
using std::vector;

Emitter::Emitter(RT &rt) noexcept
: ir(ctx), rt(rt)
{
}

llvm::Value *
Emitter::emit(
        AST const &ast,
        llvm::Module &module,
        RTScope &scope)
{
    switch (ast.getType()) {

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *value = scope.slots[ident];

        if (!value) {
            throw SyntaxError(
                    "Slot " + ident + " for assignment does not exist");
        }

        if (value->ir) {
            throw SyntaxError(
                    "Slot " + ident + " has already been assigned");
        }

        value->ir = emit(stmt.rhs, module, scope);
        return value->ir;
    }

    case ASTType::ExprDecLit: {
        auto &expr = cast<DecLitExpr>(ast);
        return llvm::ConstantFP::get(ctx, llvm::APFloat(expr.value));
    }

    case ASTType::ExprIntLit: {
        // XXX: Proper integers.
        auto &expr = cast<IntLitExpr>(ast);
        return llvm::ConstantFP::get(ctx, llvm::APFloat((double) expr.value));
    }

    case ASTType::ExprIdent: {
        auto &ident = cast<IdentExpr>(ast);
        auto *value = scope.slots[ident.name];

        if (!value) {
            throw SyntaxError("Slot " + ident.name + " does not exist");
        }

        if (!value->ir) {
            throw SyntaxError(
                    "IR for identifier " + ident.name +
                    " has not been emitted");
        }

        return value->ir;
    }

    case ASTType::ExprCall: {
        auto &call = cast<CallExpr>(ast);
        auto *callee = emit(call.lhs, module, scope);
        vector<llvm::Value *> args;
        for (auto arg : call.args) {
            args.push_back(emit(*arg, module, scope));
        }
        return ir.CreateCall(callee, args, "call");
    }

    case ASTType::ExprLambda: {
        auto &lambda = cast<LambdaExpr>(ast);
        RTScope &inner = rt.createScope(scope);

        for (auto arg : lambda.args) {
            inner.addSlot(*arg);
        }

        // TODO: Support types other than double.
        vector<llvm::Type *> types(
                lambda.args.size(),
                llvm::Type::getDoubleTy(ctx));

        auto *fty = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(ctx),
                types,
                false);

        auto *f = llvm::Function::Create(
                fty,
                llvm::Function::ExternalLinkage,
                "lambda",
                &module);

        int iArg = 0;
        for (auto &arg : f->args()) {
            auto &name = *lambda.args[iArg];
            arg.setName(name);
            inner.slots[name]->ir = &arg;
        }

        auto *bb = llvm::BasicBlock::Create(ctx, "start", f);

        ir.SetInsertPoint(bb);

        llvm::Value *value = emit(lambda.body, module, inner);
        ir.CreateRet(value);
        llvm::verifyFunction(*f);
        return f;
    }

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(expr.lhs, module, scope);
        auto *rhs = emit(expr.rhs, module, scope);

        // TODO: At this point we'd have to type-check.

        switch (expr.op) {
        case tok_add: return ir.CreateFAdd(lhs, rhs, "add");
        default: return nullptr;
        }

    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(expr.expr, module, scope);
    }

    default:
        throw EmitterError(
                "Unrecognised ASTType " + to_string(
                        static_cast<int>(ast.getType())));
    }
}

llvm::Module *
Emitter::emitModule(
        vector<Stmt *> const &stmts,
        string const &name)
{
    llvm::Module &module = *new llvm::Module(name, ctx);
    RTScope &scope = rt.createScope();

    // Scan the module for assignment to module-level slots,
    // which is used to pre-populate global scope.
    //
    // Then emit a special slot, __body__, to which the body
    // statements are bound. __body__ is executed as soon as
    // the module has been loaded.

    for (auto stmt : stmts) {
        switch (stmt->getType()) {

        case ASTType::StmtAssign: {
            // Pre-register slots referenced in assignments.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            scope.slots.insert(make_pair(ident, new RTValue()));
            break;
        }

        default: break;
        }
    }

    // Emit bytecode for the module prototype and body.
    vector<llvm::Type *> types;

    llvm::FunctionType *fty =
            llvm::FunctionType::get(
                    llvm::Type::getDoubleTy(ctx),
                    types,
                    false);

    llvm::Function *f =
            llvm::Function::Create(
                    fty,
                    llvm::Function::ExternalLinkage,
                    "__body__",
                    &module);

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "start", f);
    llvm::Value *value = nullptr;

    for (auto stmt : stmts) {
        ir.SetInsertPoint(bb);
        value = emit(*stmt, module, scope);
    }

    ir.CreateRet(value);
    llvm::verifyFunction(*f);

    return &module;
}
