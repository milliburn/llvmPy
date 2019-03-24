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

    REQUIRE(tokens.size() >= 1);
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
    auto actual = result ? result->toString() : "";
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

TEST_CASE("Parser4: literals") {
    SECTION("Integer") {
        et("1", "1i");
    }

    SECTION("Decimal") {
        et("1.0", "1d");
        et("2.5", "2.5d");
    }

    SECTION("String") {
        et("\"Test\"", "\"Test\"");
        et("'Test'", "\"Test\"");
        et("\"\"", "\"\"");
        et("''", "\"\"");
    }
}

TEST_CASE("Parser4: expressions") {
    SECTION("Empty") {
        // TODO: Fails currently, as the leading indent isn't emitted.
        // et("", "");
    }

    SECTION("Line termination") {
        et("1\n2", "1i");
        // et("\n1\n2", ""); // TODO
    }

    SECTION("Unary") {
        et("-1", "-1i");
        et("+1", "+1i");
        et("-1.0", "-1d");
        et("+1.0", "+1d");
        et("-2.5", "-2.5d");
        et("+2.5", "+2.5d");
    }

    SECTION("Binary") {
        SECTION("Expressions with one binary operator") {
            std::string const operators[] = {
                    "+", "-", "*", "/",
                    "<", "<=", "==", "!=", ">=", ">",
            };

            for (auto &op : operators) {
                et("1 " + op + " 2", "(1i " + op + " 2i)");
            }
        }

        SECTION("Expressions with two binary operators") {
            et("1 + 2 + 3", "((1i + 2i) + 3i)");
            et("1 * 2 + 3", "((1i * 2i) + 3i)");
            et("1 + 2 * 3", "(1i + (2i * 3i))");
            et("1 < 2 + 3", "(1i < (2i + 3i))");
        }

        SECTION("Expressions with unary negatives") {
            et("1 + - 2", "(1i + -2i)");
            et("1 + -2", "(1i + -2i)");
            et("1 +- 2", "(1i + -2i)");
            et("1 +-2", "(1i + -2i)");
            et("1+-2", "(1i + -2i)");
            et("1+- 2", "(1i + -2i)");
        }

        SECTION("Expressions with three binary operators") {
            et("1 + 2 + 3 + 4", "(((1i + 2i) + 3i) + 4i)");
            et("1 + 2 * 3 + 4", "((1i + (2i * 3i)) + 4i)");
            et("1 + 2 * 3 * 4", "(1i + ((2i * 3i) * 4i))");
            et("1 * 2 * 3 + 4", "(((1i * 2i) * 3i) + 4i)");
            et("1 * 2 + 3 * 4", "((1i * 2i) + (3i * 4i))");
        }

        SECTION("Expressions with two or three binary operators and parentheses") {
            et("1 + (2 + 3)", "(1i + (2i + 3i))");
            et("(1 + 2) + 3", "((1i + 2i) + 3i)");
            et("((1 + 2) + 3)", "((1i + 2i) + 3i)");
            et("1 * (2 + 3)", "(1i * (2i + 3i))");
            et("(1 * 2) + 3", "((1i * 2i) + 3i)");
            et("((1 * 2) + 3)", "((1i * 2i) + 3i)");
            et("(1 + 2) * 3", "((1i + 2i) * 3i)");
            et("(1 < 2) + 3", "((1i < 2i) + 3i)");
            et("1 + 2 * (3 + 4)", "(1i + (2i * (3i + 4i)))");
        }
    }

    SECTION("Tuples") {
        SECTION("Flat") {
            et("()", "()");
            et("(1,)", "(1i,)");
            et("(1, 2)", "(1i, 2i)");
            et("(1, 2, 3)", "(1i, 2i, 3i)");
            et("1,", "(1i,)");
            et("1, 2", "(1i, 2i)");
            et("1, 2,3", "(1i, 2i, 3i)");
        }

        SECTION("Nested") {
            et("((1, 2), 3)", "((1i, 2i), 3i)");
            et("(1, (2, 3))", "(1i, (2i, 3i))");
            et("(1, 2, (3, 4, 5))", "(1i, 2i, (3i, 4i, 5i))");
            et("(1, 2, (3, 4, 5), 6, 7)", "(1i, 2i, (3i, 4i, 5i), 6i, 7i)");
        }

        SECTION("Mixed") {
            // Some of these examples would not be valid syntax, but any such
            // errors wouldn't occur at the parser level.
            et("(1 * 2 + 3, 4 + 5 * 6, 7 + 8 + 9)",
                  "(((1i * 2i) + 3i), (4i + (5i * 6i)), ((7i + 8i) + 9i))");
            et("(1, 2 + 3, 4 + (5, 6), 7)",
                  "(1i, (2i + 3i), (4i + (5i, 6i)), 7i)");
        }
        //
        // SECTION("With lambdas") {
        //     check("lambda x: x + 1,", "(lambda x: x + 1,)");
        //     check("lambda x: x + 1, lambda y: y + 2",
        //           "((lambda x: (x + 1i)), (lambda y: (y + 2i)))");
        //     check("(lambda x: x + 1, lambda y: y + 2)",
        //           "((lambda x: (x + 1i)), (lambda y: (y + 2i)))");
        // }
    }
}
