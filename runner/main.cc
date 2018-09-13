#include <llvmPy/Lexer.h>
#include <llvmPy/Parser.h>
#include <llvmPy/Emitter.h>
#include <fstream>
#include <iostream>

int
main(int argc, char * argv[])
{
    if (argc != 2) {
        std::cerr << "The file name must be provided as the first argument.\n";
        return 1;
    }

    std::fstream fp(argv[1], std::fstream::in);

    llvmPy::Lexer lexer(fp);
    std::vector<llvmPy::Token> tokens;

    if (!lexer.tokenize(tokens)) {
        std::cerr << "Tokenisation failure\n";
        return 1;
    }

    llvmPy::Parser parser(tokens);
    llvmPy::Stmt *stmt;

    if (!parser.parse(stmt)) {
        std::cerr << "Parser failure\n";
        return 1;
    }

    return 0;
}
