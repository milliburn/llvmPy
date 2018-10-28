#include <llvmPy.h>
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
