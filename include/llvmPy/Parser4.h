#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <unordered_map>

#ifdef __cplusplus
namespace llvmPy {

/**
 * Operator precedence is implemented by way of precedence climbing. A good
 * introduction on the subject is https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing.
 */
class Parser4 {
public:
    Parser4(std::vector<Token> const &tokens);

    /** This should be the go-to for external consumers. */
    CompoundStmt &Module();

    Expr *Expression();
    Stmt *Statement();

private:
    bool EndOfFile();
    bool EndOfLine();
    int NextIndentation();

    TokenType UnaryOperator();
    TokenType BinaryOperator();
    std::vector<std::string> FunctionArguments();
    int Indentation();

    Expr *ValueExpression(int minimumPrecedence);
    Expr *Subexpression();
    Expr *UnaryExpression();
    Expr *BinaryExpression(int minimumPrecedence, Expr &lhs);
    Expr *LambdaExpression();
    Expr *CallExpression(Expr &callee);
    Expr *GetattrExpression(Expr &object);

    IdentExpr *Identifier();
    Expr *NumericLiteral();
    Expr *StringLiteral();

    Stmt *Statement(int outerIndent);

    Stmt *SimpleStatement();
    Stmt *ExpressionStatement();
    Stmt *BreakStatement();
    Stmt *ContinueStatement();
    Stmt *PassStatement();
    Stmt *ReturnStatement();
    Stmt *AssignStatement();

    Stmt *BlockStatement(int outerIndent);
    CompoundStmt *CompoundStatement(int outerIndent);
    Stmt *WhileStatement(int outerIndent);
    Stmt *IfStatement(int outerIndent);
    Stmt *IfStatement(int outerIndent, bool isElif);
    Stmt *DefStatement(int outerIndent);

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

    void syntax(bool condition, std::string const &message);
};

} // namespace llvmPy
#endif // __cplusplus
