#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class Parser {
public:
    explicit Parser(std::vector<Token> &);
    bool parse(Stmt &);
    bool parseStmt(Stmt &);
    bool parseExpr(Expr &);

private:
    std::vector<Token> & tokens;
    std::vector<Token>::iterator iter;
    Token lasttoken;

    void next();

    typeof(iter) save();
    void restore(typeof(iter));

    Token & last();
    bool is(TokenType);
    bool is_a(TokenType);
    bool is(TokenType, std::string const &);
    void want(bool);

    void endOfStmt();
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
