#include <llvmPy/Lexer.h>
#include <llvmPy/Parser.h>
#include <llvmPy/Emitter.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/RT.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <string>
#include <vector>
#include <catch2/catch.hpp>
using namespace llvmPy;
using std::istringstream;
using std::string;
using std::vector;

static char const *PROG1_SRC =
        "x = 1 \n";

static char const *PROG2_SRC =
        "x = lambda x: x + 1 \n"
        "y = x(2) + 1 \n"
        "y \n";

static void
emit(char const *prog)
{
    RT rt;
    Compiler compiler(rt);
    Emitter emitter(rt);

    string str(prog);
    istringstream ss(str);

    Lexer lexer(ss);
    vector<Token> tokens;
    lexer.tokenize(tokens);

    Parser parser(tokens);
    vector<Stmt *> stmts;
    parser.parse(stmts);

    llvm::Module *module = emitter.emitModule(stmts, "__main__");
    module->print(llvm::errs(), nullptr);
}

TEST_CASE("Emitter", "[Emitter]") {
    emit(PROG1_SRC);
    emit(PROG2_SRC);
}
