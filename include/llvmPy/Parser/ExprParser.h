#pragma once

#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <memory>
#include <istream>
#include <deque>

#ifdef __cplusplus
namespace llvmPy {

/**
 * This class is specialized towards parsing the AST of a single expression
 * (which may be a complex tree) out of a stream of tokens.
 */
class ExprParser {
public:
    typedef std::vector<Token>::iterator & TTokenIter;

    static std::unique_ptr<Expr> fromIter(TTokenIter iter);

    explicit ExprParser(TTokenIter tokens);
    std::unique_ptr<Expr> parse();

    IntLitExpr *findIntegerLiteral();
    IdentExpr *findIdentifier();
    TokenExpr *findOperator();
    void consume();

protected:
    bool is(TokenType tokenType);
    bool is_a(TokenType tokenType);
    void next();
    void back();

private:
    TTokenIter iter;
    std::deque<Expr *> output;
    std::deque<Expr *> operators;
    Token const &token() const;


};

} // namespace llvmPy
#endif // __cplusplus
