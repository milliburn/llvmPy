#include <assert.h>
#include <llvmPy/Compiler/Parser/AstPass.h>
using namespace llvmPy;

AstPass::AstPass(std::string const &phaseName)
    : Phase(phaseName)
{
}

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
    }
}

void
AstPass::updateExprStmt(ExprStmt &stmt)
{
}
