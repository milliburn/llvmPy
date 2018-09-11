#pragma once
#include <iostream>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum class TokenType {
    IDENT,
    LITER,
    OPER,
    SYNTAX,
    INDENT,
    KEYW,
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
    MUL,
    DIV,
    LAMBDA,
    NOT,
    EQUALS,
};

enum class SyntaxType {
    END_LINE,
    END_FILE,
    L_PAREN,
    R_PAREN,
    COLON,
    ASSIGN,
    SEMICOLON,
    DOT,
};

enum class KeywType {
    LAMBDA,
    PASS,
    DEFUN,
    IMPORT,
    FROM,
    IF,
    ELSE,
    ELIF,
};

class Token {
public:
    TokenType const tokenType;
    static Token * parse(std::istream&);
    virtual std::string toString() = 0;
    bool isEOL() const;
    bool isEOF() const;

protected:
    explicit Token(TokenType);
};

class Tokenizer {
public:
    explicit Tokenizer(std::istream &);
    std::vector<Token *> tokenize();

private:
    std::istream & stream;
    std::vector<Token *> tokens;
    char ch;
    char buf[65];
    int ibuf;
    int ilast;

    void next();
    void reset();
    void push();
    void pop();

    void expect(bool);
    bool is(char);
    bool isnot(char);
    bool oneof(char const *);
    bool is(int(*)(int));

    bool number();
    bool string();
    bool keywOrIdent();
    bool oper();
};

class Ident : public Token {
public:
    std::string const name;
    explicit Ident(std::string);
    std::string toString() override;
};

class Liter : public Token {
public:
    LiterType const literType;
    explicit Liter(LiterType);
    std::string toString() override;

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
    std::string toString() override;
};

class Syntax : public Token {
public:
    SyntaxType const syntaxType;
    explicit Syntax(SyntaxType);
    std::string toString() override;
};

class Keyw : public Token {
public:
    KeywType const keywType;
    explicit Keyw(KeywType);
    std::string toString() override;
};

class Indent : public Token {
public:
    int const depth;
    explicit Indent(int depth);
    std::string toString() override;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
