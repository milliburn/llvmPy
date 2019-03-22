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
    virtual void updateAst(AST &ast);

    virtual void updateExpr(Expr &expr);
    virtual void updateLambdaExpr(LambdaExpr &lambda);

    virtual void updateStmt(Stmt &stmt);
    virtual void updateExprStmt(ExprStmt &stmt);
};


} // namespace llvmPy
#endif // __cplusplus
