#include <llvmPy/Lexer.h>
#include <llvmPy/Parser/ExprParser.h>
#include <catch2/catch.hpp>
#include <string>
#include <sstream>
using namespace llvmPy;

static std::vector<Token>
tokenize(std::string input)
{
    std::istringstream stream(input);
    Lexer lexer(stream);
    std::vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    REQUIRE(tokens.size() > 1);
    REQUIRE(tokens[0].type == tok_indent);
    // Remove the indent marker (expressions don't expect it).
    tokens.erase(tokens.begin());
    return tokens;
}

// static ExprParser
// start(std::string input)
// {
//     std::vector<Token> tokens = tokenize(input);
//     ExprParser parser(tokens.begin(), tokens.end());
//     return parser;
// }

// static std::unique_ptr<Expr>
// parse(std::string input)
// {
//     ExprParser parser = start(input);
//     auto result = parser.parse();
//     return result;
// }

static std::string
parseToString(std::string input)
{
    auto tokens = tokenize(input);
    auto begin = tokens.begin();
    auto expr = ExprParser::fromIter(begin, tokens.end());
    std::ostringstream ss;
    ss << *expr;
    return ss.str();
}

TEST_CASE("ExprParser", "[ExprParser]") {
    SECTION("Literals") {
        SECTION("Integer") {
            CHECK(parseToString("1") == "1i");
            CHECK(parseToString("-1") == "-1i");
            CHECK(parseToString("+1") == "1i");
        }

        SECTION("Identifier") {
            CHECK(parseToString("True") == "True");
        }
    }

    SECTION("It should parse binary expressions with one operator") {
        CHECK(parseToString("1 + 2") == "(1i + 2i)");
        CHECK(parseToString("1 - 2") == "(1i - 2i)");
        CHECK(parseToString("1 * 2") == "(1i * 2i)");
        CHECK(parseToString("1 / 2") == "(1i / 2i)");
    }
}
