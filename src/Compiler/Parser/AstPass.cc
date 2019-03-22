#include <assert.h>
#include <llvmPy/Compiler/Parser/AstPass.h>
using namespace llvmPy;

AstPass::AstPass(std::string const &phaseName)
    : Phase(phaseName), _isUpdated(false)
{
}

void
AstPass::run(AST &ast)
{
    do {
        setUpdated(false);
        updateAst(ast);
    } while (isUpdated());
}

bool
AstPass::isUpdated() const
{
    return _isUpdated;
}

void
AstPass::setUpdated(bool value)
{
    _isUpdated = value;
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
    if (auto *str = expr.cast<StringExpr>()) {
        updateStringExpr(*str);
    } else if (auto *dec = expr.cast<DecimalExpr>()) {
        updateDecimalExpr(*dec);
    } else if (auto *inte = expr.cast<IntegerExpr>()) {
        updateIntegerExpr(*inte);
    } else if (auto *ident = expr.cast<IdentExpr>()) {
        updateIdentExpr(*ident);
    } else if (auto *lambda = expr.cast<LambdaExpr>()) {
        updateLambdaExpr(*lambda);
    } else if (auto *unary = expr.cast<UnaryExpr>()) {
        updateUnaryExpr(*unary);
    } else if (auto *binary = expr.cast<BinaryExpr>()) {
        updateBinaryExpr(*binary);
    } else if (auto *call = expr.cast<CallExpr>()) {
        updateCallExpr(*call);
    } else if (auto *token = expr.cast<TokenExpr>()) {
        updateTokenExpr(*token);
    } else if (auto *tuple = expr.cast<TupleExpr>()) {
        updateTupleExpr(*tuple);
    } else if (auto *getattr = expr.cast<GetattrExpr>()) {
        updateGetattrExpr(*getattr);
    } else {
        assert(false);
    }
}

void
AstPass::updateStringExpr(StringExpr &expr)
{
}

void
AstPass::updateDecimalExpr(DecimalExpr &expr)
{
}

void
AstPass::updateIntegerExpr(IntegerExpr &expr)
{
}

void
AstPass::updateIdentExpr(IdentExpr &expr)
{
}

void
AstPass::updateLambdaExpr(LambdaExpr &expr)
{
    updateExpr(expr.getExpr());
}

void
AstPass::updateUnaryExpr(UnaryExpr &unary)
{
    updateExpr(unary.getExpr());
}

void
AstPass::updateBinaryExpr(BinaryExpr &binary)
{
    updateExpr(binary.getLeftOperand());
    if (isUpdated()) return;
    updateExpr(binary.getRightOperand());
}

void
AstPass::updateCallExpr(CallExpr &call)
{
    updateExpr(call.getCallee());

    for (auto &arg : call.args()) {
        updateExpr(arg);
        if (isUpdated()) return;
    }
}

void
AstPass::updateTokenExpr(TokenExpr &token)
{
}

void
AstPass::updateTupleExpr(TupleExpr &tuple)
{
    for (auto &member : tuple.members()) {
        updateExpr(member);
        if (isUpdated()) return;
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
    } else if (auto *pass = stmt.cast<PassStmt>()) {
        updatePassStmt(*pass);
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        updateConditionalStmt(*cond);
    } else if (auto *loop = stmt.cast<WhileStmt>()) {
        updateWhileStmt(*loop);
    } else if (auto *brk = stmt.cast<BreakStmt>()) {
        updateBreakStmt(*brk);
    } else if (auto *cont = stmt.cast<ContinueStmt>()) {
        updateContinueStmt(*cont);
    } else {
        assert(false);
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
        if (isUpdated()) return;
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
AstPass::updatePassStmt(PassStmt &pass)
{
}

void
AstPass::updateConditionalStmt(ConditionalStmt &cond)
{
    updateExpr(cond.getCondition());
    if (isUpdated()) return;
    updateStmt(cond.getThenBranch());
    if (isUpdated()) return;
    updateStmt(cond.getElseBranch());
}

void
AstPass::updateWhileStmt(WhileStmt &loop)
{
    updateExpr(loop.getCondition());
    if (isUpdated()) return;
    updateStmt(loop.getBody());
}

void
AstPass::updateBreakStmt(BreakStmt &brk)
{
}

void
AstPass::updateContinueStmt(ContinueStmt &cont)
{
}
