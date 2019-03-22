#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler/Phase.h>

#ifdef __cplusplus
namespace llvmPy {

class AstPass : public Phase<AST &, void> {
public:
    AstPass(std::string const &phaseName);
    void run(AST &ast) override;

protected:
    bool isUpdated() const;
    void setUpdated(bool value);

    virtual void updateAst(AST &ast);

    virtual void updateExpr(Expr &expr);
    virtual void updateStringExpr(StringExpr &expr);
    virtual void updateDecimalExpr(DecimalExpr &expr);
    virtual void updateIntegerExpr(IntegerExpr &expr);
    virtual void updateIdentExpr(IdentExpr &expr);
    virtual void updateLambdaExpr(LambdaExpr &lambda);
    virtual void updateUnaryExpr(UnaryExpr &unary);
    virtual void updateBinaryExpr(BinaryExpr &binary);
    virtual void updateCallExpr(CallExpr &call);
    virtual void updateTokenExpr(TokenExpr &token);
    virtual void updateTupleExpr(TupleExpr &tuple);
    virtual void updateGetattrExpr(GetattrExpr &getattr);

    virtual void updateStmt(Stmt &stmt);
    virtual void updateExprStmt(ExprStmt &stmt);
    virtual void updateAssignStmt(AssignStmt &assign);
    virtual void updateCompoundStmt(CompoundStmt &compound);
    virtual void updateDefStmt(DefStmt &def);
    virtual void updateReturnStmt(ReturnStmt &ret);
    virtual void updatePassStmt(PassStmt &pass);
    virtual void updateConditionalStmt(ConditionalStmt &cond);
    virtual void updateWhileStmt(WhileStmt &loop);
    virtual void updateBreakStmt(BreakStmt &brk);
    virtual void updateContinueStmt(ContinueStmt &cont);

    template<class T>
    T &getParentOfType(AST &ast) const {
        AST &parent = ast.getParent();
        if (auto *it = parent.cast<T>()) {
            return *it;
        } else {
            return getParentOfType<T>(parent);
        }
    }

private:
    bool _isUpdated;
};


} // namespace llvmPy
#endif // __cplusplus
