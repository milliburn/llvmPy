#include <llvmPy.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
using namespace llvmPy;
namespace cl = llvm::cl;
using std::cerr;
using std::cout;
using std::endl;
using std::string;

cl::opt<string> Cmd(
        "c",
        cl::desc("Program passed as input"),
        cl::value_desc("cmd"),
        cl::Required);

int
main(int argc, char **argv)
{
    cl::ParseCommandLineOptions(argc, argv);

    std::vector<Token> tokens;
    std::vector<Stmt *> stmts;

    std::stringstream ss(Cmd);
    Lexer lexer(ss);
    lexer.tokenize(tokens);

    Parser parser(tokens);
    parser.parse(stmts);

    RT rt;
    Compiler compiler(rt);
    Emitter em(compiler);

    RTModule &mod = *em.createModule("", stmts);
    mod.getModule().print(llvm::outs(), nullptr);

    return 0;
}
