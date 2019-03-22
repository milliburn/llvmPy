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
    } else if (auto *binary = expr.cast<BinaryExpr>()) {
        updateBinaryExpr(*binary);
    } else if (auto *call = expr.cast<CallExpr>()) {
        updateCallExpr(*call);
    } else if (auto *tuple = expr.cast<TupleExpr>()) {
        updateTupleExpr(*tuple);
    } else if (auto *getattr = expr.cast<GetattrExpr>()) {
        updateGetattrExpr(*getattr);
    }
}

void
AstPass::updateLambdaExpr(LambdaExpr &expr)
{
    updateExpr(expr.getExpr());
}

void
AstPass::updateBinaryExpr(BinaryExpr &binary)
{
    updateExpr(binary.getLeftOperand());
    updateExpr(binary.getRightOperand());
}

void
AstPass::updateCallExpr(CallExpr &call)
{
    updateExpr(call.getCallee());

    for (auto &arg : call.args()) {
        updateExpr(arg);
    }
}

void
AstPass::updateTupleExpr(TupleExpr &tuple)
{
    for (auto &member : tuple.members()) {
        updateExpr(member);
    }
}

void
AstPass::updateGetattrExpr(GetattrExpr &getattr)
{
    updateExpr(getattr.getObject());
}

void
AstPass::updateStmt(Stmt &stmt)
{
    if (auto *expr = stmt.cast<ExprStmt>()) {
        updateExprStmt(*expr);
    } else if (auto *assign = stmt.cast<AssignStmt>()) {
        updateAssignStmt(*assign);
    } else if (auto *compound = stmt.cast<CompoundStmt>()) {
        updateCompoundStmt(*compound);
    } else if (auto *def = stmt.cast<DefStmt>()) {
        updateDefStmt(*def);
    } else if (auto *ret = stmt.cast<ReturnStmt>()) {
        updateReturnStmt(*ret);
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        updateConditionalStmt(*cond);
    } else if (auto *loop = stmt.cast<WhileStmt>()) {
        updateWhileStmt(*loop);
    }
}

void
AstPass::updateExprStmt(ExprStmt &stmt)
{
    updateExpr(stmt.getExpr());
}

void
AstPass::updateAssignStmt(AssignStmt &assign)
{
    updateExpr(assign.getValue());
}

void
AstPass::updateCompoundStmt(CompoundStmt &compound)
{
    for (auto &stmt : compound.statements()) {
        updateStmt(stmt);
    }
}

void
AstPass::updateDefStmt(DefStmt &def)
{
    updateStmt(def.getBody());
}

void
AstPass::updateReturnStmt(ReturnStmt &ret)
{
    updateExpr(ret.getExpr());
}

void
AstPass::updateConditionalStmt(ConditionalStmt &cond)
{
    updateExpr(cond.getCondition());
    updateStmt(cond.getThenBranch());
    updateStmt(cond.getElseBranch());
}

void
AstPass::updateWhileStmt(WhileStmt &loop)
{
    updateExpr(loop.getCondition());
    updateStmt(loop.getBody());
}
