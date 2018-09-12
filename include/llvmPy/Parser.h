#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class Parser {
public:
    explicit Parser(std::vector<Token> &);
    bool parse(Stmt * &);

private:
    bool parse_(Stmt * &);
    bool parse_(Expr * &);

    std::vector<Token> & tokens;
    std::vector<Token>::iterator iter;
    Token * lasttoken;

    void next();

    typeof(iter) save();
    void restore(typeof(iter));

    Token & last();
    bool is(TokenType);
    bool is_a(TokenType);
    void want(bool);

    void endOfStmt();
};

} // namespace llvmPy
#endif // __cplusplus
