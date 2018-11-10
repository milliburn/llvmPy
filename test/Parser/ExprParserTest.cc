#include <llvmPy/Lexer.h>
#include <llvmPy/Parser/ExprParser.h>
#include <string>
#include <sstream>
#include <llvm/Support/Casting.h>
#include <catch2/catch.hpp>
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

static ExprParser
start(std::string input)
{
    std::istringstream stream(input);
    Lexer lexer(stream);
    std::vector<Token> tokens;
    REQUIRE(lexer.tokenize(tokens));
    REQUIRE(tokens.size() > 1);
    REQUIRE(tokens[0].type == tok_indent);
    // Remove the indent marker (expressions don't expect it).
    tokens.erase(tokens.begin());
    ExprParser parser(tokens.begin(), tokens.end());
    return parser;
}

static std::unique_ptr<Expr>
parse(std::string input)
{
    ExprParser parser = start(input);
    auto result = parser.parse();
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

TEST_CASE("ExprParser algorithm", "[ExprParserAlgo]") {
    SECTION("Binary expression") {
        auto tokens = tokenize("1 + 2");
        ExprParser p(tokens.begin(), tokens.end());

        // When the parser is initialised, the output and operator
        // stacks are empty.

        REQUIRE(p.getOutput().empty());
        REQUIRE(p.getOperators().empty());

        // The first consumption results in a literal.

        p.consume();
        REQUIRE(p.getOutput().size() == 1);
        REQUIRE(p.getOutput().top()->toString() == "1i");
        REQUIRE(p.getOperators().size() == 0);

        // The second consumption is an operator.

        p.consume();
        REQUIRE(p.getOutput().size() == 1);
        REQUIRE(p.getOperators().size() == 1);
        REQUIRE(llvm::cast<TokenExpr>(
                p.getOperators().top())->getTokenType()
                == tok_add);

        // The third consumption is a literal.

        p.consume();
        REQUIRE(p.getOutput().size() == 2);
        REQUIRE(p.getOutput().top()->toString() == "2i");
        REQUIRE(p.getOperators().size() == 1);
    }
}

TEST_CASE("ExprParser", "[ExprParser]") {
    SECTION("It should parse literals") {
        // CHECK(parseToString("1") == "1i");
        // CHECK(parseToString("True") == "True");
    }

    SECTION("It should parse binary operations") {
        CHECK(parseToString("1 + 2") == "(1i + 2i)");
    }
}
