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

    for (int i = 0; i < tokens.size(); ++i) {
        if (i > 0
            && tokens[i].type != tok_eol
            && tokens[i-1].type != tok_eol) {
            
            output += ' ';
        }

        ostringstream ss;
        ss << tokens[i];
        output += ss.str();
    }

    return output;
}

TEST_CASE("Lexer", "[Lexer]") {
    SECTION("Numbers") {
        REQUIRE(tokenize("2") == ">0 2n ;eof");
        REQUIRE(tokenize("2.0") == ">0 2.0n ;eof");
        REQUIRE(tokenize("-2") == ">0 - 2n ;eof");
    }

    SECTION("Strings") {
        REQUIRE(tokenize("\"Hello!\"") == ">0 \"Hello!\" ;eof");

        // Preserve the type of string delimiter used
        REQUIRE(tokenize("\'Hello!\'") == ">0 \'Hello!\' ;eof");

        // Preserve escaped delimiters
        REQUIRE(tokenize("\"Escaped \\\"String\\\"\"") ==
                ">0 \"Escaped \\\"String\\\"\" ;eof");

        // Ensure the lexer can handle strings longer than its internal buffer.
        string str(Lexer::BUFFER_SIZE * 2, 'x');
        REQUIRE(tokenize("\"" + str + "\"") == ">0 \"" + str + "\" ;eof");
    }

    SECTION("Identifiers") {
        REQUIRE(tokenize("xyz") == ">0 xyz ;eof");
        REQUIRE(tokenize("True") == ">0 True ;eof");
        REQUIRE(tokenize("False") == ">0 False ;eof");
        REQUIRE(tokenize("None") == ">0 None ;eof");
        REQUIRE(tokenize("_under_score_123") == ">0 _under_score_123 ;eof");
        REQUIRE(tokenize("def") == ">0 def ;eof");
    }

    SECTION("Operators and syntax") {
        REQUIRE(tokenize("*") == ">0 * ;eof");
        REQUIRE(tokenize("(*)") == ">0 ( * ) ;eof");
        REQUIRE(tokenize("+=") == ">0 += ;eof");
        REQUIRE(tokenize("+ =") == ">0 + = ;eof");
    }

    SECTION("Multiple statements") {
        REQUIRE(tokenize("x = 1 \ny = x + 1 \ny \n") ==
                ">0 x = 1n\n>0 y = x + 1n\n>0 y\n;eof");
    }
}
