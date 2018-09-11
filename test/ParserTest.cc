#include <array>
#include <llvmPy/AST.h>
#include <string>
#include <sstream>
#include <utility>
#include <catch2/catch.hpp>
using namespace llvmPy::AST;
using namespace std;

static string
expr(string input)
{
    istringstream stream(input);
    Parser p(stream);
    p.tokenize();
    return p.expr()->toString();
}

TEST_CASE("Parser") {
    SECTION("Simple expressions") {
        REQUIRE(expr("1") == "1i");
        REQUIRE(expr("1.0") == "1.000000d");
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
