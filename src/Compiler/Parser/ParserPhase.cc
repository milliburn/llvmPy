#include <assert.h>
#include <llvmPy/Compiler/Parser/ParserPhase.h>
#include <llvmPy/Parser2.h>
using namespace llvmPy;

static std::string const phaseName = "parser";

ParserPhase::ParserPhase()
    : Phase(phaseName)
{

}

std::unique_ptr<Stmt>
ParserPhase::run(std::vector<Token> &tokens)
{
    auto iter = tokens.begin();
    Parser2 parser(iter, tokens.end());
    return parser.read();
}
