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
Emitter::emit(AST const &ast, RTScope &scope)
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

        value->ir = emit(stmt.rhs, scope);
        return value->ir;
    }

    case ASTType::ExprNumLitDec: {
        auto &expr = cast<DecLitExpr>(ast);
        return llvm::ConstantFP::get(ctx, llvm::APFloat(expr.value));
    }

    case ASTType::ExprNumLitInt: {
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

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(expr.lhs, scope);
        auto *rhs = emit(expr.rhs, scope);

        // TODO: At this point we'd have to type-check.

        switch (expr.op) {
        case tok_add: return ir.CreateFAdd(lhs, rhs, "add");
        default: return nullptr;
        }

    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(expr.expr, scope);
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
    vector<Stmt *> body;
    llvm::Module &module = *new llvm::Module(name, ctx);
    RTScope scope(rt);

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

    for (auto stmt : stmts) {
        body.push_back(stmt);
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
    ir.SetInsertPoint(bb);

    for (auto stmt : body) {
        value = emit(*stmt, scope);
    }

    ir.CreateRet(value);
    llvm::verifyFunction(*f);

    return &module;
}
