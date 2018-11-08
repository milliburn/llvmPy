#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <string>
#include <map>

#ifdef __cplusplus
namespace llvmPy {

class Parser {
public:
    explicit Parser(std::vector<Token> &);

    void   parse(std::vector<Stmt *> &);
    Stmt * parseStmt();
    Expr * parseExpr();
    void   parseEndOfStmt();
    LitExpr * parseLitExpr();

private:
    std::vector<Token> & tokens;
    std::vector<Token>::iterator iter;
    std::map<TokenType, int> const precedence;

    Token * lasttoken;

    TokenType current();
    void next();

    typeof(iter) save();
    void restore(typeof(iter));

    Token & last();
    bool is(TokenType);
    bool is_a(TokenType);

    void want(TokenType);
    void want_a(TokenType);

};

} // namespace llvmPy
#endif // __cplusplus
