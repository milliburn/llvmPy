#include <llvmPy.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
using namespace llvmPy;
using namespace llvm::cl;
using std::cout;
using std::string;

opt<bool> IsNaked("naked", desc("Output IR without module decorations"));
opt<string> Cmd("c", desc("Program passed as input"), value_desc("cmd"));

int
main(int argc, char **argv)
{
    ParseCommandLineOptions(argc, argv);

    std::vector<Token> tokens;
    std::vector<Stmt *> stmts;

    if (!Cmd.empty()) {
        std::stringstream ss(Cmd);
        Lexer lexer(ss);
        lexer.tokenize(tokens);
    } else {
        return 127;
    }

    Parser parser(tokens);
    parser.parse(stmts);

    RT rt;
    Compiler compiler(rt);
    Emitter em(compiler);

    if (IsNaked) {
        RTModule &mod = *em.createModule("", stmts);
        mod.getModule().print(llvm::outs(), nullptr);
    }

    return 0;
}
