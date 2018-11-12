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
    DefStmt *findDefStatement(int indent);
    ReturnStmt *findReturnStatement();

    CallExpr *buildCall(Expr *lhs, Expr *rhs);

    Expr *readExpr(int precedence = 0, Expr *lhs = nullptr);
    Expr *readSubExpr();
    Stmt *readStatement();
    CompoundStmt *readCompoundStatement(int outerIndent);

    bool isEnd();

protected:
    bool is(TokenType tokenType);
    bool is_a(TokenType tokenType);
    void next();
    void back();

private:
    TTokenIter &iter;
    TTokenIter iter_end;
    Token const &token() const;

    int getPrecedence(TokenType tokenType) const;
    int getPrecedence(TokenExpr *tokenExpr) const;

    std::map<TokenType, int> const precedences;
};

} // namespace llvmPy
#endif // __cplusplus
