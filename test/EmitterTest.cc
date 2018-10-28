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
using llvm::Value;

static char const *PROG1_SRC =
        "z = lambda x: x + 1 \n"
        "y = 1 \n"
        "x = y + 1 \n";

static char const *PROG2_SRC =
        "x = lambda x: x + 1 \n"
        "y = x(2) + 1 \n"
        "y \n";

static char const *PROG3_SRC = "3\n";

static void
emit(char const *prog)
{
    RT rt;
    Compiler compiler(rt);
    Emitter emitter(compiler);

    string str(prog);
    istringstream ss(str);

    Lexer lexer(ss);
    vector<Token> tokens;
    lexer.tokenize(tokens);

    Parser parser(tokens);
    vector<Stmt *> stmts;
    parser.parse(stmts);

    auto *module = emitter.emitModule(stmts, "__main__");
    module->ir->print(llvm::errs(), nullptr);
}

static Stmt *
parseStmt(char const *str)
{
    string istr(str);
    istringstream ss(istr);

    Lexer lexer(ss);
    vector<Token> tokens;
    lexer.tokenize(tokens);

    Parser parser(tokens);
    vector<Stmt *> stmts;
    parser.parse(stmts);

    // REQUIRE(stmts.size() == 1);
    return stmts.front();
}

static vector<Stmt *>
parseStmts(char const *str)
{
    // string istr(str);
    istringstream ss(str);

    Lexer lexer(ss);
    vector<Token> tokens;
    lexer.tokenize(tokens);

    Parser parser(tokens);
    vector<Stmt *> stmts;
    parser.parse(stmts);

    return stmts;
}

TEST_CASE("Emitter", "[Emitter]") {
    RT rt;
    Compiler compiler(rt);
    Emitter em(compiler);
    llvm::Module *module = new llvm::Module("main", compiler.getContext());
    RTScope scope;

    SECTION("ExprDecLit") {
        vector<Stmt *> stmts = parseStmts("1");
        auto &mod = *em.createModule("");
        llvm::Value *v = em.emit(mod, stmts);
        // auto *mod = em.emitModuleNaked(stmts);
        // Value *v = em.emitValue(*stmt, *module, scope);
        mod.getModule().print(llvm::errs(), nullptr);
    }

    SECTION("Assign statement") {
        vector<Stmt *> stmts = parseStmts("x = 1");
        auto &mod = *em.createModule("");
        llvm::Value *v = em.emit(mod, stmts);
        mod.getModule().print(llvm::errs(), nullptr);
    }
}
