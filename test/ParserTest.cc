#include <array>
#include <llvmPy/Lexer.h>
#include <llvmPy/Parser.h>
#include <string>
#include <sstream>
#include <catch2/catch.hpp>
using namespace llvmPy::AST;
using namespace std;

static string
expr(string input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    Parser parser(tokens);
    Stmt stmt;
    REQUIRE(parser.parseStmt(stmt));
    stringstream ss;
    ss << stmt;
    return ss.str();
}

TEST_CASE("Parser", "[Parser]") {
    SECTION("Numeric literals") {
        REQUIRE(expr("1") == "1i");
        REQUIRE(expr("2.5") == "2.5d");
    }

    SECTION("Identifiers") {
        REQUIRE(expr("True") == "1b");
        REQUIRE(expr("False") == "0b");
        REQUIRE(expr("x") == "x");
        REQUIRE(expr("abc") == "abc");
    }

    SECTION("Binary expressions") {
        REQUIRE(expr("1 + 2") == "(1i+2i)");
        REQUIRE(expr("x + 1") == "(x+1i)");
        REQUIRE(expr("lambda: x + 1") == "(lambda: (x+1))");
    }
}
