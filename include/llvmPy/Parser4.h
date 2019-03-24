#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <unordered_map>

#ifdef __cplusplus
namespace llvmPy {

class Parser4 {
public:
    Parser4(std::vector<Token> const &tokens);

    Expr *Expression();
    Stmt *Statement();

private:
    bool EndOfFile();
    bool EndOfLine();

    TokenType UnaryOperator();
    TokenType BinaryOperator();

    Expr *Expression(int minimumPrecedence);
    Expr *UnaryExpression();

    Expr *NumericLiteral();
    Expr *StringLiteral();


private:
    Token const * const _tokens;
    size_t const _tokenCount;
    size_t _index;
    std::unordered_map<TokenType, int> const _precedence;

    void next();
    void back();

    bool is(TokenType type);
    Token const &peek() const;
    bool peek(TokenType type) const;
    Token const &take();

    int precedence(Token const &token) const;
    int precedence(TokenType type) const;
    bool isLeftAssociative(Token const &token) const;
    bool isLeftAssociative(TokenType type) const;
};

} // namespace llvmPy
#endif // __cplusplus
