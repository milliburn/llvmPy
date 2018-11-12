#include <llvmPy/Lexer.h>
#include <llvmPy/Parser2.h>
#include <string>
#include <sstream>
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

static std::string
parseToString(std::string input)
{
    auto tokens = tokenize(input);
    auto begin = tokens.begin();
    auto expr = Parser2::fromIter(begin, tokens.end());
    std::ostringstream ss;

    if (expr) {
        ss << *expr;
    }

    return ss.str();
}

static void
check(std::string input, std::string expect)
{
    INFO("For: " << input);
    auto actual = parseToString(input);
    CHECK(actual == expect);
}

TEST_CASE("Parser2", "[Parser2]") {
    SECTION("Literals") {
        SECTION("Integers") {
            check("1", "1i");
            check("-1", "-1i");
            check("+1", "+1i");
        }

        SECTION("Decimals") {
            check("1.0", "1d");
            check("-1.0", "-1d");
            check("+1.0", "+1d");
            check("2.5", "2.5d");
        }

        SECTION("Identifiers") {
            check("True", "True");
            check("False", "False");
            check("x", "x");
        }

        SECTION("Parentheses") {
            check("()", "()"); // Empty tuple.
            check("(1)", "1i"); // Single value (no tuple).
            check("(-1)", "-1i");
            check("(True)", "True");
        }

        SECTION("Strings") {
            check("\"Test\"", "\"Test\"");
            check("'Test'", "\"Test\"");
            check("\"\"", "\"\"");
            check("''", "\"\"");
        }

        SECTION("Lambda expressions") {
            check("lambda: None", "(lambda: None)");
            check("lambda: x + 1", "(lambda: (x + 1i))");
            check("lambda x: x + 1", "(lambda x: (x + 1i))");
            check("lambda x, y: x + 1", "(lambda x, y: (x + 1i))");
        }
    }

    SECTION("Tuples") {
        SECTION("Flat") {
            check("()", "()");
            check("(1,)", "(1i,)");
            check("(1, 2)", "(1i, 2i)");
            check("(1, 2, 3)", "(1i, 2i, 3i)");
        }

        SECTION("Nested") {
            check("((1, 2), 3)", "((1i, 2i), 3i)");
            check("(1, (2, 3))", "(1i, (2i, 3i))");
            check("(1, 2, (3, 4, 5))", "(1i, 2i, (3i, 4i, 5i))");
            check("(1, 2, (3, 4, 5), 6, 7)", "(1i, 2i, (3i, 4i, 5i), 6i, 7i)");
        }

        SECTION("Mixed") {
            // Some of these examples would not be valid syntax, but any such
            // errors wouldn't occur at the parser level.
            check("(1 * 2 + 3, 4 + 5 * 6, 7 + 8 + 9)",
                  "(((1i * 2i) + 3i), (4i + (5i * 6i)), ((7i + 8i) + 9i))");
            check("(1, 2 + 3, 4 + (5, 6), 7)",
                  "(1i, (2i + 3i), (4i + (5i, 6i)), 7i)");
        }
    }

    SECTION("Function call") {
        check("f()", "f()");
        check("f(1)", "f(1i)");
        check("f(1, 2)", "f(1i, 2i)");
        check("f(x + 1, 2)", "f((x + 1i), 2i)");
        check("print(1 != 2)", "print((1i != 2i))");
    }

    SECTION("Expressions with one binary operator") {
        std::string const operators[] = {
                "+", "-", "*", "/",
                "<", "<=", "==", "!=", ">=", ">",
        };

        for (auto &op : operators) {
            check("1 " + op + " 2", "(1i " + op + " 2i)");
        }
    }

    SECTION("Expressions with two binary operators") {
        check("1 + 2 + 3", "((1i + 2i) + 3i)");
        check("1 * 2 + 3", "((1i * 2i) + 3i)");
        check("1 + 2 * 3", "(1i + (2i * 3i))");
        check("1 < 2 + 3", "(1i < (2i + 3i))");
    }

    SECTION("Expressions with embedded negatives") {
        check("1 + - 2", "(1i + -2i)");
        check("1 + -2", "(1i + -2i)");
        check("1 +- 2", "(1i + -2i)");
        check("1 +-2", "(1i + -2i)");
        check("1+-2", "(1i + -2i)");
        check("1+- 2", "(1i + -2i)");
    }

    SECTION("Expressions with three binary operators") {
        check("1 + 2 + 3 + 4", "(((1i + 2i) + 3i) + 4i)");
        check("1 + 2 * 3 + 4", "((1i + (2i * 3i)) + 4i)");
        check("1 + 2 * 3 * 4", "(1i + ((2i * 3i) * 4i))");
        check("1 * 2 * 3 + 4", "(((1i * 2i) * 3i) + 4i)");
        check("1 * 2 + 3 * 4", "((1i * 2i) + (3i * 4i))");
    }

    SECTION("Expressions with two or three binary operators and parentheses") {
        check("1 + (2 + 3)", "(1i + (2i + 3i))");
        check("(1 + 2) + 3", "((1i + 2i) + 3i)");
        check("((1 + 2) + 3)", "((1i + 2i) + 3i)");
        check("1 * (2 + 3)", "(1i * (2i + 3i))");
        check("(1 * 2) + 3", "((1i * 2i) + 3i)");
        check("((1 * 2) + 3)", "((1i * 2i) + 3i)");
        check("(1 + 2) * 3", "((1i + 2i) * 3i)");
        check("(1 < 2) + 3", "((1i < 2i) + 3i)");
        check("1 + 2 * (3 + 4)", "(1i + (2i * (3i + 4i)))");
    }
}
