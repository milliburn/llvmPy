#include <array>
#include <llvmPy/AST.h>
#include <string>
#include <sstream>
#include <utility>
#include <catch2/catch.hpp>
using namespace llvmPy::AST;
using namespace std;

static string
parseExpr(string input)
{
    istringstream stream(input);
    Parser p(stream);
    p.tokenize();
    return p.expr()->toString();
}

TEST_CASE("Parser") {
    SECTION("Simple expressions") {
        array<pair<string, string>, 6> tests = {
                make_pair("1", "1i"),
                make_pair("1.0", "1.000000d"),
                make_pair("True", "1b"),
                make_pair("False", "0b"),
                make_pair("x", "x"),
                make_pair("abc", "abc"),
        };

        for (auto pair : tests) {
            REQUIRE(parseExpr(pair.first) == pair.second);
        }
    }

    SECTION("Binary expressions") {
        array<pair<string, string>, 3> tests = {
                make_pair("1 + 2", "(1i+2i)"),
                make_pair("x+ 1", "(x+1i)"),
                make_pair("x = 1", "(x=1i)"),
        };

        for (auto pair : tests) {
            REQUIRE(parseExpr(pair.first) == pair.second);
        }
    }
}
