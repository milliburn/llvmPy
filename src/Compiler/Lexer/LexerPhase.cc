#include <assert.h>
#include <llvmPy/Compiler/Lexer/LexerPhase.h>
#include <llvmPy/Lexer.h>
using namespace llvmPy;

static std::string const phaseName = "lexer";

LexerPhase::LexerPhase()
    : Phase(phaseName)
{
}

std::vector<Token>
LexerPhase::run(std::istream &input)
{
    std::vector<Token> tokens;
    Lexer lexer(input);
    lexer.tokenize(tokens);
    return tokens;
}
