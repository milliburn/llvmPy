#pragma once
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum class TokenType {
    EOL,
    NUM_LIT,
    STR_LIT,
    OPER,
    LPARENS,
    RPARENS,
};

class Token {
public:
    static Token * parse(std::istream&);
    TokenType getType();
    virtual std::string toString();

protected:
    Token(TokenType);
    TokenType const tokenType;
};

class NumLit : public Token {
public:
    double datum;
    NumLit(double);
    virtual std::string toString() override;
};

class StrLit : public Token {
public:
    char * datum;
};

class Oper : public Token {
public:
    int datum;
    Oper(int);
    virtual std::string toString() override;
};

class EOL : public Token {
public:
    EOL();
    virtual std::string toString() override;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
