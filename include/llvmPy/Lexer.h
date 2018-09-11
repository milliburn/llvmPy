#pragma once
#include <llvmPy/Token.h>
#include <string>
#include <sstream>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class Token; // llvmPy/AST/Token.h

class Lexer {
public:
    static constexpr long BUFFER_SIZE = 64;

    explicit Lexer(std::istream &);
    bool tokenize(std::vector<Token> &);
    bool isError();
    long getLine();
    long getColumn();
    std::string getError();
    bool isEof();

private:
    std::istream & stream;
    std::vector<Token> tokens;
    char ch;
    char buf[BUFFER_SIZE];
    long ibuf;
    long ilast;

    long line;
    long col;
    std::string error;

    void add(Token);

    void next();
    void next(std::stringstream &);
    void reset();
    void push();
    void pop();

    bool is(char);
    bool is(char, std::stringstream &);
    bool oneof(char const *);
    bool oneof(char const *, std::stringstream &);
    bool oneof(int(*)(int));
    bool oneof(int(*)(int), std::stringstream &);

    bool numlit();
    bool strlit();
    bool ident();
    bool syntax();
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
