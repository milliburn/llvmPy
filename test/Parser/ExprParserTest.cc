#include <catch2/catch.hpp>
#include <llvmPy/Lexer.h>
#include <llvmPy/Parser/ExprParser.h>
#include <string>
#include <sstream>
using namespace llvmPy;

static std::unique_ptr<Expr>
parse(std::string input)
{
    std::istringstream stream(input);
    Lexer lexer(stream);
    std::vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    REQUIRE(tokens.size() > 1);
    REQUIRE(tokens[0].type == tok_indent);
    // Remove the indent marker (expressions don't expect it).
    tokens.erase(tokens.begin());
    auto iter = tokens.begin();
    auto result = ExprParser::fromIter(iter);
    return result;
}

static std::string
parseToString(std::string input)
{
    auto expr = parse(input);
    std::ostringstream ss;
    ss << *expr;
    return ss.str();
}

TEST_CASE("ExprParser", "[ExprParser]") {
    SECTION("It should parse literals") {
        CHECK(parseToString("1") == "1i");
        // CHECK(parseToString("True") == "True");
    }
}
