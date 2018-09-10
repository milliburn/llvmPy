#pragma once
#include <llvmPy/AST/Token.h>
#include <llvmPy/AST/Expr.h>
#include <llvmPy/AST/Stmt.h>
#include <iostream>
#include <vector>
#include <stack>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class Parser {
public:
    Parser(std::istream &);
    bool tokenize();
    Stmt * stmt();
    Expr * expr();

private:
    std::istream & stream;
    std::vector<Token *> tokens;
    std::vector<Token *>::iterator iter;
    std::stack<typeof(iter)> memory;

    void next();
    void push();
    void pop();
    void discard();

    bool is(TokenType);
    bool is(SyntaxType);
    bool is(KeywType);
    void expect(TokenType);
    void expect(SyntaxType);
    void expect(KeywType);

    void endOfStmt();
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
