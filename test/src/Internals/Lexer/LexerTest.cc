#include <llvmPy/Lexer.h>
#include <string>
#include <sstream>
#include <catch2/catch.hpp>
using namespace llvmPy;
using namespace std;

static std::vector<TokenType>
types(string const &input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    lexer.tokenize(tokens);

    vector<TokenType> result;
    for (auto &token : tokens) {
        result.push_back(token.getTokenType());
    }

    return result;
}

static string
tokenize(string const &input)
{
    istringstream stream(input);
    Lexer lexer(stream);
    vector<Token> tokens;
    lexer.tokenize(tokens);
    string output;
    int eofcount = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].getTokenType() == tok_eof) {
            eofcount++;
        }

        if (i > 0
            && tokens[i].getTokenType() != tok_eol
            && tokens[i].getTokenType() != tok_eof
            && tokens[i-1].getTokenType() != tok_eol) {

            output += ' ';
        }

        ostringstream ss;
        ss << tokens[i];
        output += ss.str();
    }


    // EOF is not printable, but ensure it's the last token.
    REQUIRE(eofcount == 1);
    REQUIRE(tokens.back().getTokenType() == tok_eof);

    return output;
}

TEST_CASE("Lexer", "[Lexer]") {
    SECTION("EOF and EOL marker rules") {
        CHECK(types("") == vector<TokenType>({
            tok_eof }));
        CHECK(types("\n\n\n") == vector<TokenType>({
            tok_eof }));
        CHECK(types("2") == vector<TokenType>({
            tok_indent, tok_number, tok_eol, tok_eof }));
        CHECK(types("2\n") == vector<TokenType>({
            tok_indent, tok_number, tok_eol, tok_eof }));
        CHECK(types("2\n2") == vector<TokenType>({
            tok_indent, tok_number, tok_eol,
            tok_indent, tok_number, tok_eol,
            tok_eof }));
        CHECK(types("2\n\n2\n") == vector<TokenType>({
            tok_indent, tok_number, tok_eol,
            tok_indent, tok_number, tok_eol,
            tok_eof }));
    }

    SECTION("Numbers") {
        CHECK(tokenize("2") == ">0 2n\n");
        CHECK(tokenize("2.0") == ">0 2.0n\n");
        CHECK(tokenize("-2") == ">0 - 2n\n");
    }

    SECTION("Strings") {
        CHECK(tokenize("\"Hello!\"") == ">0 \"Hello!\"\n");

        // Preserve the type of string delimiter used
        CHECK(tokenize("\'Hello!\'") == ">0 \'Hello!\'\n");

        // Preserve escaped delimiters
        CHECK(tokenize("\"Escaped \\\"String\\\"\"") ==
                ">0 \"Escaped \\\"String\\\"\"\n");

        // Ensure the lexer can handle strings longer than its internal buffer.
        string str(Lexer::BUFFER_SIZE * 2, 'x');
        CHECK(tokenize("\"" + str + "\"") == ">0 \"" + str + "\"\n");
    }

    SECTION("Identifiers") {
        CHECK(tokenize("xyz") == ">0 xyz\n");
        CHECK(tokenize("True") == ">0 True\n");
        CHECK(tokenize("False") == ">0 False\n");
        CHECK(tokenize("None") == ">0 None\n");
        CHECK(tokenize("_under_score_123") == ">0 _under_score_123\n");
        CHECK(tokenize("def") == ">0 >def\n");
    }

    SECTION("Operators and syntax") {
        CHECK(tokenize("*") == ">0 *\n");
        CHECK(tokenize("(*)") == ">0 ( * )\n");
        CHECK(tokenize("+=") == ">0 +=\n");
        CHECK(tokenize("+ =") == ">0 + =\n");
    }

    SECTION("Multiple statements") {
        CHECK(tokenize("x = 1\nx = 2") == ">0 x = 1n\n>0 x = 2n\n");
        CHECK(tokenize("x = 1 \ny = x + 1 \ny \n") ==
              ">0 x = 1n\n>0 y = x + 1n\n>0 y\n");
    }

    SECTION("Function definitions") {
        CHECK(tokenize("def f():\n  x = y + 1\n") ==
                ">0 >def f ( ) :\n>2 x = y + 1n\n");
    }

    SECTION("Comments") {
        CHECK(tokenize("# Comment\nx = 1 \n # Comment") ==
              ">0 x = 1n\n");
        CHECK(tokenize("# Comment \nx = 1 \n # Comment \n") ==
              ">0 x = 1n\n");
        CHECK(tokenize("# Comment \nx = 1 \n # Comment \nx = 2") ==
              ">0 x = 1n\n>0 x = 2n\n");
        CHECK(tokenize("x = 1 #Comment \n # Comment \nx = 2") ==
              ">0 x = 1n\n>0 x = 2n\n");
    }
}
