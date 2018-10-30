#include <llvmPy.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "testutil.h"
using namespace llvmPy;
using std::ifstream;
using std::string;
using std::vector;

vector<Stmt *> llvmPy::parseFile(string const &path)
{
    ifstream fp;
    fp.open(path, std::ios::in);

    if (!fp.is_open()) {
        throw "File not found!";
    }

    vector<Token> tokens;
    vector<Stmt *> stmts;

    Lexer lexer(fp);
    lexer.tokenize(tokens);
    Parser parser(tokens);
    parser.parse(stmts);

    return stmts;
}

string llvmPy::readFile(string const &path)
{
    ifstream fp;
    fp.open(path, std::ios::in);

    if (!fp.is_open()) {
        throw "File not found!";
    }

    std::stringstream sstr;
    sstr << fp.rdbuf();
    return sstr.str();
}

string llvmPy::emitFile(string const &path)
{
    RT rt;
    Compiler compiler(rt);
    Emitter em(compiler);

    RTModule &mod = *em.createModule("");
    em.emit(mod.getScope(), parseFile(path));

    string out;
    llvm::raw_string_ostream sstr(out);
    mod.getModule().print(sstr, nullptr);

    return out;
}
