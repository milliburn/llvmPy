#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
using namespace llvmPy;
using llvm::cast;
using std::make_pair;
using std::string;

Emitter::Emitter(RT &rt)
: ir(ctx), rt(rt)
{
}

llvm::Value *
Emitter::emit(AST const &ast, RTScope &scope)
{
    switch (ast.getType()) {

    case AST::stmt_assign: {
        auto &stmt = cast<AssignStmt>(ast);
        string const &ident = stmt.lhs;
        auto *rhs = emit(stmt.rhs, scope);
        auto *value = new RTValue(rhs);
        scope.slots.insert(make_pair(ident, value));
        return nullptr;
    }

    case AST::expr_numlit_dec: {
        auto &expr = cast<DecLitExpr>(ast);
        return llvm::ConstantFP::get(ctx, llvm::APFloat(expr.value));
    }

    case AST::expr_ident: {
        auto &ident = cast<IdentExpr>(ast);
        auto *value = scope.slots[ident.name];
        if (!value) return nullptr;
        return value->ir;
    }

    case AST::expr_binary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(expr.lhs, scope);
        auto *rhs = emit(expr.rhs, scope);

        // TODO: At this point we'd have to type-check.

        switch (expr.op) {
        case tok_add: return ir.CreateFAdd(lhs, rhs, "add");
        default: return nullptr;
        }

    }

    default:
        return nullptr;
    }
}
