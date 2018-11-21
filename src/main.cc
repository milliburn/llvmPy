#include <llvmPy.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace llvmPy;
namespace cl = llvm::cl;
using std::cerr;
using std::cout;
using std::endl;
using std::string;

static cl::opt<string> Cmd(
        "c",
        cl::desc("Program passed as string."),
        cl::value_desc("cmd"));

static cl::opt<bool> IsIR(
        "ir",
        cl::desc("Print resulting LLVM IR and exit."));

static cl::opt<bool> IsParser(
        "parser",
        cl::desc("Print resulting parser tree and exit."));

static cl::opt<bool> IsLexer(
        "lexer",
        cl::desc("Print tokens output by the lexer and exit."));

static cl::opt<string> Filename(
        cl::Positional,
        cl::desc("file"));

int
main(int argc, char **argv)
{
    cl::ParseCommandLineOptions(argc, argv);

    std::vector<Token> tokens;

    if (Cmd.getPosition() && Filename.getPosition()) {
        cerr << "Only one of cmd or filename may be specified." << endl;
        exit(1);
    } else if (Cmd.getPosition()) {
        std::stringstream ss(Cmd);
        Lexer lexer(ss);
        lexer.tokenize(tokens);
    } else if (Filename.getPosition()) {
        std::ifstream input;
        input.open(Filename, std::ios::in);

        if (input.fail()) {
            // File not found.
            cerr << "Cannot open file '"
                    << Filename << "': file not found."
                    << endl;
            exit(1);
        }

        Lexer lexer(input);
        lexer.tokenize(tokens);
    } else {
        Lexer lexer(std::cin);
        lexer.tokenize(tokens);
    }

    if (IsLexer) {
        int iTokenOnLine = 0;
        for (auto const &token : tokens) {
            if (iTokenOnLine > 0
                    && token.getTokenType() != tok_eol
                    && token.getTokenType() != tok_eof) {
                cout << ' ';
            }

            if (token.getTokenType() == tok_eof) {
                // Right now EOF doesn't have a canonical representation.
                cout << endl;
                cout << ">EOF";
            } else {
                cout << token;
            }

            iTokenOnLine += 1;

            if (token.getTokenType() == tok_eol) {
                iTokenOnLine = 0;
            }
        }

        return 0;
    }

    auto iter = tokens.begin();
    Parser2 parser(iter, tokens.end());
    auto stmt = parser.read();

    if (IsParser) {
        std::cout << *stmt;
        return 0;
    }

    Compiler compiler;
    Emitter em(compiler);
    RT rt(compiler);

    RTModule &mod = *em.createModule("__main__", *stmt);

    if (IsIR) {
        mod.getModule().print(llvm::outs(), nullptr);
    } else {
        rt.import(mod);
    }

    return 0;
}
