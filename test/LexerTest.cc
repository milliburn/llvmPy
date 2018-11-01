#include <llvmPy/Lexer.h>
#include <string>
#include <sstream>
#include <catch2/catch.hpp>
using namespace llvmPy;
using namespace std;

static string
tokenize(string input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    lexer.tokenize(tokens);
    string output;
    int eofcount = 0;

    for (int i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == tok_eof) {
            eofcount++;
        }

        if (i > 0
            && tokens[i].type != tok_eol
            && tokens[i].type != tok_eof
            && tokens[i-1].type != tok_eol) {

            output += ' ';
        }

        ostringstream ss;
        ss << tokens[i];
        output += ss.str();
    }


    // EOF is not printable, but ensure it's the last token.
    REQUIRE(eofcount == 1);
    REQUIRE(tokens.back().type == tok_eof);

    return output;
}

TEST_CASE("Lexer", "[Lexer]") {
    SECTION("Numbers") {
        REQUIRE(tokenize("2") == ">0 2n");
        REQUIRE(tokenize("2.0") == ">0 2.0n");
        REQUIRE(tokenize("-2") == ">0 - 2n");
    }

    SECTION("Strings") {
        REQUIRE(tokenize("\"Hello!\"") == ">0 \"Hello!\"");

        // Preserve the type of string delimiter used
        REQUIRE(tokenize("\'Hello!\'") == ">0 \'Hello!\'");

        // Preserve escaped delimiters
        REQUIRE(tokenize("\"Escaped \\\"String\\\"\"") ==
                ">0 \"Escaped \\\"String\\\"\"");

        // Ensure the lexer can handle strings longer than its internal buffer.
        string str(Lexer::BUFFER_SIZE * 2, 'x');
        REQUIRE(tokenize("\"" + str + "\"") == ">0 \"" + str + "\"");
    }

    SECTION("Identifiers") {
        REQUIRE(tokenize("xyz") == ">0 xyz");
        REQUIRE(tokenize("True") == ">0 True");
        REQUIRE(tokenize("False") == ">0 False");
        REQUIRE(tokenize("None") == ">0 None");
        REQUIRE(tokenize("_under_score_123") == ">0 _under_score_123");
        REQUIRE(tokenize("def") == ">0 def");
    }

    SECTION("Operators and syntax") {
        REQUIRE(tokenize("*") == ">0 *");
        REQUIRE(tokenize("(*)") == ">0 ( * )");
        REQUIRE(tokenize("+=") == ">0 +=");
        REQUIRE(tokenize("+ =") == ">0 + =");
    }

    SECTION("Multiple statements") {
        CHECK(tokenize("x = 1\nx = 2") == ">0 x = 1n\n>0 x = 2n");
        CHECK(tokenize("x = 1 \ny = x + 1 \ny \n") ==
              ">0 x = 1n\n>0 y = x + 1n\n>0 y");
    }

    SECTION("Function definitions") {
        REQUIRE(tokenize("def f():\n  x = y + 1\n") ==
                ">0 def f ( ) :\n>2 x = y + 1n");
    }

    SECTION("Comments") {
        CHECK(tokenize("# Comment\nx = 1 \n # Comment") ==
              ">0 x = 1n");
        CHECK(tokenize("# Comment \nx = 1 \n # Comment \n") ==
              ">0 x = 1n");
        CHECK(tokenize("# Comment \nx = 1 \n # Comment \nx = 2") ==
              ">0 x = 1n\n>0 x = 2n");
        CHECK(tokenize("x = 1 #Comment \n # Comment \nx = 2") ==
              ">0 x = 1n\n>0 x = 2n");
    }
}
