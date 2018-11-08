#include <array>
#include <llvmPy/Lexer.h>
#include <llvmPy/Parser.h>
#include <string>
#include <sstream>
#include <vector>
#include <catch2/catch.hpp>
using namespace llvmPy;
using namespace std;

static string
expr(string input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    Parser parser(tokens);
    Stmt *stmt = parser.parseStmt();
    REQUIRE(stmt != nullptr);
    stringstream ss;
    ss << *stmt;
    return ss.str();
}

static string
module(string input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    Parser parser(tokens);
    vector<Stmt *> stmts;
    parser.parse(stmts);
    stringstream ss;

    for (int i = 0; i < stmts.size(); ++i) {
        if (i > 0) {
            ss << "\n";
        }

        ss << *stmts[i];
    }

    return ss.str();
}

TEST_CASE("Parser", "[Parser]") {
    SECTION("Numeric literals") {
        REQUIRE(expr("1") == "1i");
        REQUIRE(expr("-1") == "-1i");
        REQUIRE(expr("2.5") == "2.5d");
    }

    SECTION("Identifiers") {
        REQUIRE(expr("True") == "True");
        REQUIRE(expr("False") == "False");
        REQUIRE(expr("x") == "x");
        REQUIRE(expr("abc") == "abc");
    }

    SECTION("Binary expressions") {
        REQUIRE(expr("1 + 2") == "(1i + 2i)");
        REQUIRE(expr("1 - 2") == "(1i - 2i)");
        REQUIRE(expr("1 + - 2") == "(1i + -2i)");
        REQUIRE(expr("x + 1") == "(x + 1i)");
    }

    SECTION("Lambda expressions") {
        REQUIRE(expr("lambda: x + 1") == "(lambda: (x + 1i))");
        REQUIRE(expr("lambda x: x + 1") == "(lambda x: (x + 1i))");
        REQUIRE(expr("lambda x, y: x + 1") == "(lambda x, y: (x + 1i))");
    }

    SECTION("Small modules") {
        REQUIRE(module(" x =  1 \ny = x + 1\ny\n") ==
                "x = 1i\ny = (x + 1i)\ny");
    }

    SECTION("Function calls") {
        REQUIRE(expr("f()") == "f()");
        REQUIRE(expr("f(1)") == "f(1i)");
        REQUIRE(expr("f(x+1, 2)") == "f((x + 1i), 2i)");
        REQUIRE(expr("print(1 != 2)") == "print((1i != 2i))");
    }

    SECTION("Function definitions 1") {
        string f1_in =
                "def func():\n"
                "    y = x + 1\n"
                "    z = 9 + 2\n";

        string f1_out =
                "def func():\n"
                "  y = (x + 1i)\n"
                "  z = (9i + 2i)";

        REQUIRE(expr(f1_in) == f1_out);
    }

    // SECTION("Function definitions 2") {
    //     string input =
    //             "def func(x):\n"
    //             "    y = x + 1\n"
    //             "    return y + 2\n"
    //             "\n"
    //             "print(func(1))\n";
    //
    //     string expect =
    //             "def func(x):\n"
    //             "  y = (x + 1i)\n"
    //             "  return (y + 2i)\n"
    //             "print(func(1i))\n";
    //
    //     REQUIRE(expr(input) == expect);
    // }
}
