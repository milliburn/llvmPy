#include <llvmPy/Compiler/Lexer/LexerPhase.h>
#include <llvmPy/Parser4.h>
#include <catch2.h>
using namespace llvmPy;

static std::vector<Token>
tokenize(std::string const &input, bool trim)
{
    static LexerPhase lexer;
    std::istringstream inputStream(input);
    std::vector<Token> tokens = lexer.run(inputStream);

    REQUIRE(tokens.size() > 1);
    REQUIRE(tokens[0].getTokenType() == tok_indent);
    REQUIRE(tokens[tokens.size() - 1].getTokenType() == tok_eof);

    if (trim) {
        // Remove the indent marker (expressions don't expect it).
        // Can't use tokens.erase(), as that would require a copy
        // assignment operator on Token.

        std::vector<Token> result;
        for (size_t i = 1; i < tokens.size(); ++i) {
            result.push_back(std::move(tokens[i]));
        }

        return result;
    } else {
        return tokens;
    }
}

static void
et(std::string const &input, std::string const &expect)
{
    INFO("For: " << input);
    auto tokens = tokenize(input, true);
    Parser4 parser(tokens);
    Expr *result = parser.Expression();
    REQUIRE(result);
    auto actual = result->toString();
    REQUIRE(actual == expect);
}

// static void
// st(std::string const &input, std::string const &expect)
// {
//     INFO("For:\n" << input);
//     auto tokens = tokenize(input, false);
//     Parser4 parser(tokens);
//     Stmt *result = parser.Statement();
//     REQUIRE(result);
//     auto actual = result->toString();
//     REQUIRE(actual == expect);
// }

TEST_CASE("Parser4: expressions") {
    SECTION("Integer literals") {
        et("1", "1i");
        // et("-1", "-1i");
        // et("+1", "+1i");
    }

    SECTION("Decimal literals") {
        et("1.0", "1d");
        // et("-1.0", "-1d");
        // et("+1.0", "+1d");
        et("2.5", "2.5d");
    }

    SECTION("Strings") {
        et("\"Test\"", "\"Test\"");
        et("'Test'", "\"Test\"");
        et("\"\"", "\"\"");
        et("''", "\"\"");
    }
}
