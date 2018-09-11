#include <array>
#include <llvmPy/AST.h>
#include <string>
#include <sstream>
#include <utility>
#include <catch2/catch.hpp>
using namespace llvmPy::AST;
using namespace std;

static string
tokenize(string input)
{
    istringstream stream(input);
    Tokenizer tok(stream);
    vector<Token *> tokens = tok.tokenize();
    string output;

    for (int i = 0; i < tokens.size(); ++i) {
        if (i > 0)
            output += ' ';

        output += tokens[i]->toString();
    }

    return output;
}

TEST_CASE("Tokenizer", "[Tokenizer]") {
    SECTION("Simple literals and identifiers") {
        // Numbers
        REQUIRE(tokenize("2") == ">0 2i __eof__");
        REQUIRE(tokenize("2.0") == ">0 2.000000d __eof__");
        REQUIRE(tokenize("-2") == ">0 -2i __eof__");

        // Identifiers
        REQUIRE(tokenize("xyz") == ">0 xyz* __eof__");
        REQUIRE(tokenize("True") == ">0 True* __eof__");
        REQUIRE(tokenize("False") == ">0 False* __eof__");
        REQUIRE(tokenize("None") == ">0 None* __eof__");
        REQUIRE(tokenize("longer_identifier123") ==
                ">0 longer_identifier123* __eof__");

        // Keywords
        REQUIRE(tokenize("def") == ">0 def __eof__");

        // Operators and syntax
        REQUIRE(tokenize("*") == ">0 * __eof__");
        REQUIRE(tokenize("(*)") == ">0 ( * ) __eof__");
    }
}
