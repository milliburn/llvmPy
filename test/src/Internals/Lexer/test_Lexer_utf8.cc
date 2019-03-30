#include <llvmPy/Lexer.h>
#include <catch2.h>
using namespace llvmPy;

static std::string
tokenize(std::string const &input)
{
    std::istringstream stream(input);
    Lexer lexer(stream);
    std::vector<Token> tokens;
    lexer.tokenize(tokens);
    std::string output;
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

        std::ostringstream ss;
        ss << tokens[i];
        output += ss.str();
    }


    // EOF is not printable, but ensure it's the last token.
    REQUIRE(eofcount == 1);
    REQUIRE(tokens.back().getTokenType() == tok_eof);

    return output;
}

TEST_CASE("Lexer UTF8", "[Lexer][utf8]") {
    SECTION("strings") {
        CHECK(tokenize("'Test'") == ">0 'Test'\n");
        CHECK(tokenize("'Test 😀'") == ">0 'Test 😀'\n");
    }
}
