#include <assert.h>
#include <llvmPy/Compiler/Parser/AstPass.h>
using namespace llvmPy;

void
AstPass::run(AST &ast)
{
    updateAst(ast);
}

void
AstPass::updateAst(AST &ast)
{
    if (auto *expr = ast.cast<Expr>()) {
        updateExpr(*expr);
    } else if (auto *stmt = ast.cast<Stmt>()) {
        updateStmt(*stmt);
    } else {
        assert(false);
    }
}

void
AstPass::updateExpr(Expr &expr)
{
    if (auto *lambda = expr.cast<LambdaExpr>()) {
        updateLambdaExpr(*lambda);
    } else {
        assert(false);
    }
}

void
AstPass::updateLambdaExpr(LambdaExpr &expr)
{
}

void
AstPass::updateStmt(Stmt &stmt)
{
    if (auto *expr = stmt.cast<ExprStmt>()) {
        updateExprStmt(*expr);
    } else {
        assert(false);
    }
}

void
AstPass::updateExprStmt(ExprStmt &stmt)
{
}
