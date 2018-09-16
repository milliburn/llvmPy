#include <llvmPy/Lexer.h>
#include <llvmPy/Parser.h>
#include <llvmPy/Emitter.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/RT.h>
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <iostream>
using std::fstream;
using std::vector;

static llvmPy::RT       rt;
static llvmPy::Compiler compiler(rt);
static llvmPy::Emitter  emitter(rt);

static void
import(char const *path, bool isMain)
{
    fstream fp(path, fstream::in);

    llvmPy::Lexer lexer(fp);
    vector<llvmPy::Token> tokens;
    lexer.tokenize(tokens);

    llvmPy::Parser parser(tokens);
    vector<llvmPy::Stmt *> stmts;
    parser.parse(stmts);

    llvm::Module *module = emitter.emitModule(stmts, "__main__");
    //compiler.addMain(module);
    module->print(llvm::errs(), nullptr);
}

int
main(int argc, char * argv[])
{
    if (argc != 2) {
        std::cerr << "The file name must be provided as the first argument.\n";
        return 1;
    }

    import(argv[1], true);

    return 0;
}
