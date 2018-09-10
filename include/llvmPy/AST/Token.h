#pragma once
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum class TokenType {
    IDENT,
    LITER,
    OPER,
    SYNTAX,
};

enum class LiterType {
    INT,
    DEC,
    STR,
    BOOL,
};

enum class OperType {
    ASSIGN,
    ADD,
    SUB,
    LAMBDA,
};

enum class SyntaxType {
    END_LINE,
    END_FILE,
    L_PAREN,
    R_PAREN,
    COLON,
};

class Token {
public:
    TokenType const tokenType;
    static Token * parse(std::istream&);
    virtual ~Token();

protected:
    explicit Token(TokenType);
};

class Ident : public Token {
public:
    std::string const name;
    explicit Ident(std::string);
};

class Liter : public Token {
public:
    LiterType const literType;
    explicit Liter(LiterType);

    union {
        std::string* sval; // STR
        long ival; // INT
        double dval; // DEC
        bool bval; // BOOL
    };
};

class Oper : public Token {
public:
    OperType const operType;
    explicit Oper(OperType);
};

class Syntax : public Token {
public:
    SyntaxType const syntaxType;
    explicit Syntax(SyntaxType);
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
