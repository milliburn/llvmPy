#include <llvmPy.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <unistd.h>
#include <build.h>

using namespace llvmPy;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class Stage {
    PYTHON = 0,
    IR,
    PARSER,
    LEXER,
};

enum class Mode {
    STDIN,
    COMMAND,
    SCRIPT,
};

struct Options {
    Stage stage;
    Mode mode;
    string program;
    vector<string> impl;
};

static int run(Options const &);

int
main(int argc, char **argv)
{
    Options options = {
            .stage = Stage::PYTHON,
            .mode = Mode::STDIN,
    };

    char c;

    while ((c = static_cast<char>(getopt(argc, argv, "hVc:X:"))) != -1) {
        bool isDone = false;

        switch (c) {
        case 'h':
            cout << LLVMPY_NAME << endl;
            exit(0);
        case 'V':
            cout << "Python " << LLVMPY_PYTHON << " ("
                 << LLVMPY_NAME << " " << LLVMPY_VERSION << ", "
                 << LLVMPY_TIMESTAMP << ")"
                 << endl;
            exit(0);
        case 'c':
            options.program = optarg;
            options.mode = Mode::COMMAND;
            isDone = true;
            break;
        case 'X':
            std::string impl = optarg;

            if (impl == "phase=ir") {
                options.stage = Stage::IR;
            } else if (impl == "phase=parser") {
                options.stage = Stage::PARSER;
            } else if (impl == "phase=lexer") {
                options.stage = Stage::LEXER;
            } else {
                cerr << "Unknown implementation-specific option: "
                     << impl << "." << endl;
                exit(1);
            }

            break;
        }

        if (isDone) {
            break;
        }
    }

    if (optind < argc) {
        options.mode = Mode::SCRIPT;
        options.program = argv[optind++];
    }

    return run(options);
}

static int
run(Options const &options)
{
    std::vector<Token> tokens;

    if (options.mode == Mode::COMMAND) {
        std::stringstream ss(options.program);
        Lexer lexer(ss);
        lexer.tokenize(tokens);
    } else if (options.mode == Mode::SCRIPT) {
        std::ifstream input;
        input.open(options.program, std::ios::in);

        if (input.fail()) {
            // File not found.
            cerr << "Cannot open file '"
                 << options.program << "': file not found."
                 << endl;
            exit(1);
        }

        Lexer lexer(input);
        lexer.tokenize(tokens);
    } else if (options.mode == Mode::STDIN) {
        Lexer lexer(std::cin);
        lexer.tokenize(tokens);
    }

    if (options.stage == Stage::LEXER) {
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

    if (options.stage == Stage::PARSER) {
        std::cout << *stmt;
        return 0;
    }

    Compiler compiler;
    Emitter em(compiler);
    RT rt(compiler);

    RTModule &mod = *em.createModule("__main__", *stmt);

    if (options.stage == Stage::IR) {
        mod.getModule().print(llvm::outs(), nullptr);
    } else {
        rt.import(mod);
    }

    return 0;
}
