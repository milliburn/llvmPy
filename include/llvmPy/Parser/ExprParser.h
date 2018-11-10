#pragma once

#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <memory>
#include <istream>
#include <deque>
#include <stack>

#ifdef __cplusplus
namespace llvmPy {

/**
 * This class is specialized towards parsing the AST of a single expression
 * (which may be a complex tree) out of a stream of tokens.
 */
class ExprParser {
public:
    typedef std::vector<Token>::iterator TTokenIter;

    static std::unique_ptr<Expr> fromIter(TTokenIter iter, TTokenIter end);

    explicit ExprParser(TTokenIter &&tokens, TTokenIter &&end);
    std::unique_ptr<Expr> parse();

    IntLitExpr *findIntegerLiteral();
    IdentExpr *findIdentifier();
    TokenExpr *findOperator();
    void consume();
    void evaluate();

    std::stack<Expr *> const &getOutput() const;
    std::stack<Expr *> const &getOperators() const;

protected:
    bool is(TokenType tokenType);
    bool is_a(TokenType tokenType);
    bool end();
    void next();
    void back();

private:
    TTokenIter iter;
    TTokenIter iter_end;
    std::stack<Expr *> output;
    std::stack<Expr *> operators;
    std::stack<Expr *> result;
    Token const &token() const;


};

} // namespace llvmPy
#endif // __cplusplus
