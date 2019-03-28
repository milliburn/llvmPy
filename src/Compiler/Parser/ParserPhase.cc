#include <assert.h>
#include <llvmPy/Compiler/Parser/ParserPhase.h>
#include <llvmPy/Parser.h>
using namespace llvmPy;

static std::string const phaseName = "parser";

ParserPhase::ParserPhase()
    : Phase(phaseName)
{

}

std::unique_ptr<Stmt>
ParserPhase::run(std::vector<Token> &tokens)
{
    Parser parser(tokens);
    auto &stmt = parser.Module();
    return std::unique_ptr<Stmt>(&stmt);
}
