#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <memory>
#include <map>

#ifdef __cplusplus
namespace llvmPy {

class Parser3 {
public:
    typedef std::vector<Token>::iterator TTokenIter;

    static std::unique_ptr<Expr> fromIter(TTokenIter &iter, TTokenIter end);

    explicit Parser3(TTokenIter &tokens, TTokenIter end);

    std::unique_ptr<CompoundStmt> readModule();

private:
    // Transcription of the Python EBNF grammar.

    // High-level
    Expr *Expression();
    Stmt *Statement();
    Stmt *Statement(int indent);

    // Details

    std::vector<std::string> ArgumentDefinitions();
    std::vector<Expr *> CallArguments();

    bool EndOfExpression();
    bool EndOfStatement();
    bool EndOfFile();
    bool EndOfLine();
    bool Whitespace();

    Expr *Identifier();
    Expr *NumericLiteral();
    Expr *StringLiteral();

    Expr *Expression(int mp);
    Expr *Expression(int mp, bool isTupled);
    Expr *Subexpression();
    BinaryExpr *BinaryExpression(int mp);
    LambdaExpr *LambdaExpression(int mp);
    UnaryExpr *UnaryExpression(int mp);
    TupleExpr *TupleExpression(int mp, Expr *lhs);

    CompoundStmt *CompoundStatement(int outer);

    Stmt *SimpleStatement();
    Stmt *PassStatement();
    BreakStmt *BreakStatement();
    ReturnStmt *ReturnStatement();

    Stmt *BlockStatement(int indent);
    WhileStmt *WhileStatement(int indent);

    // Strict getters.
    void ExpectIndentAt(int indent);
    int ExpectIndent();
    Expr &ExpectExpression();
    CompoundStmt &ExpectModule();

private:
    bool get(TokenType tokenType);

    bool is(TokenType tokenType);
    bool is_a(TokenType tokenType);
    TokenType peek();
    bool peek(TokenType tokenType);
    bool isAtIndent(TokenType tokenType, int indent);
    void next();
    void back();

    int getPrecedence(TokenType tokenType) const;
    int getPrecedence(TokenExpr *tokenExpr) const;
    int getUnaryPrecedence(TokenType tokenType) const;
    bool isLeftAssociative(TokenType tokenType) const;
    bool isLeftAssociative(TokenExpr *tokenExpr) const;

};

} // namespace llvmPy
#endif // __cplusplus
