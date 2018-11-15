#pragma once

#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <memory>
#include <map>

#ifdef __cplusplus
namespace llvmPy {

/**
 * This class is specialized towards parsing the AST of a single expression
 * (which may be a complex tree) out of a stream of tokens.
 */
class Parser2 {
public:
    typedef std::vector<Token>::iterator TTokenIter;

    static std::unique_ptr<Expr> fromIter(TTokenIter &iter, TTokenIter end);

    explicit Parser2(TTokenIter &tokens, TTokenIter end);
    std::unique_ptr<Expr> parse();

    Expr * findNumericLiteral();
    StrLitExpr *findStringLiteral();
    IdentExpr *findIdentifier();
    TokenExpr *findOperator();
    LambdaExpr *findLambdaExpression();
    ReturnStmt *findReturnStatement();
    AssignStmt *findAssignStatement();
    PassStmt *findPassStatement();
    DefStmt *findDefStatement(int indent);
    ConditionalStmt *findConditionalStatement(int indent, bool elif = false);
    WhileStmt *findWhileStmt(int indent);
    BreakStmt *findBreakStmt();
    ContinueStmt *findContinueStmt();

    CallExpr *buildCall(Expr *lhs, Expr *rhs);

    std::unique_ptr<Stmt> read();
    Expr *readExpr(int precedence = 0, Expr *lhs = nullptr);
    Expr *readSubExpr();
    Stmt *readStatement(int indent);
    Stmt *readSimpleStatement(int indent);
    Stmt *readBlockStatement(int indent);
    CompoundStmt *readCompoundStatement(int outerIndent);

    bool isEnd();

protected:
    bool is(TokenType tokenType);
    bool is_a(TokenType tokenType);
    bool isAtIndent(TokenType tokenType, int indent);
    void next();
    void back();

private:
    TTokenIter &iter;
    TTokenIter iter_end;
    Token const &token() const;

    int getPrecedence(TokenType tokenType) const;
    int getPrecedence(TokenExpr *tokenExpr) const;

    std::map<TokenType, int> const precedences;

    void expectIndent(int indent);
};

} // namespace llvmPy
#endif // __cplusplus
