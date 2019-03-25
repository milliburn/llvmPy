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
    CHECK(actual == expect);
}

static void
st(std::string const &input, std::string const &expect)
{
    INFO("For:\n" << input);
    auto tokens = tokenize(input, false);
    Parser4 parser(tokens);
    auto &result = parser.Module();
    auto actual = result.toString();
    REQUIRE(actual == expect);
}

static Stmt &
sast(std::string const &input)
{
    INFO("For:\n" << input);
    auto tokens = tokenize(input, false);
    Parser4 parser(tokens);
    Stmt *result = parser.Statement();
    REQUIRE(result);
    return *result;
}

TEST_CASE("Parser4") {
    SECTION("Integer literals") {
        et("1", "1i");
    }

    SECTION("Decimal literals") {
        et("1.0", "1d");
        et("2.5", "2.5d");
    }

    SECTION("String literals") {
        et("\"Test\"", "\"Test\"");
        et("'Test'", "\"Test\"");
        et("\"\"", "\"\"");
        et("''", "\"\"");
    }

    SECTION("Identifier expressions") {
        et("True", "True");
        et("False", "False");
        et("x", "x");
    }

    SECTION("Subexpressions") {
        et("()", "()"); // Empty tuple.
        et("(1)", "1i"); // Single value (no tuple).
        et("(-1)", "-1i"); // Subexpressions don't exist on their own.
        et("(True)", "True");
    }

    SECTION("Empty") {
        // TODO: Fails currently, as the leading indent isn't emitted.
        // et("", "");
    }

    SECTION("Line termination") {
        et("1\n2", "1i");
        // et("\n1\n2", ""); // TODO
    }

    SECTION("Unary expressions") {
        et("-1", "-1i");
        et("+1", "+1i");
        et("-1.0", "-1d");
        et("+1.0", "+1d");
        et("-2.5", "-2.5d");
        et("+2.5", "+2.5d");
    }

    SECTION("Binary expressions") {
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

        SECTION("With lambdas") {
            et("lambda x: x + 1,", "((lambda x: (x + 1i)),)");
            et("lambda x: x + 1, lambda y: y + 2",
               "((lambda x: (x + 1i)), (lambda y: (y + 2i)))");
            et("(lambda x: x + 1, lambda y: y + 2)",
               "((lambda x: (x + 1i)), (lambda y: (y + 2i)))");
        }
    }

    SECTION("Lambda expressions") {
        et("lambda: None", "(lambda: None)");
        et("lambda: x + 1", "(lambda: (x + 1i))");
        et("lambda x: x + 1", "(lambda x: (x + 1i))");
        et("lambda x: x + 1", "(lambda x: (x + 1i))");
        // TODO: Should be an error.
        // et("lambda (x): x + 1", "(lambda x: (x + 1i))");
        et("lambda x, y: x + 1", "(lambda x, y: (x + 1i))");
    }

    SECTION("Function call expressions") {
        et("f()", "f()");
        et("f(1)", "f(1i)");
        et("f(1, 2)", "f(1i, 2i)");
        et("f(x + 1, 2)", "f((x + 1i), 2i)");
        et("print(1 != 2)", "print((1i != 2i))");
    }

    SECTION("Getattr expressions") {
        et("x.y", "x.y");
        et("x.y()", "x.y()");
        et("x.y.z()", "x.y.z()");
    }

    SECTION("Break statements") {
        st("break", "break\n");
        auto &stmt = sast("break");
        CHECK(stmt.isa<BreakStmt>());
    }

    SECTION("Continue statements") {
        st("continue", "continue\n");
        auto &stmt = sast("continue");
        CHECK(stmt.isa<ContinueStmt>());
    }

    SECTION("Pass statements") {
        st("pass", "pass\n");
        auto &stmt = sast("pass");
        CHECK(stmt.isa<PassStmt>());
    }

    SECTION("Return statements") {
        st("return 1 + 2", "return (1i + 2i)\n");
    }

    SECTION("Assign statements") {
        st("x = 1 + 2", "x = (1i + 2i)\n");
    }

    SECTION("While statements") {
        SECTION("Flat") {
            st("while True:\n break", "while True:\n    break\n");
            st("while 1 + 2:\n 2\n 3", "while (1i + 2i):\n    2i\n    3i\n");
        }

        SECTION("Nested") {
            st("while True:\n x = y + 1\n while False:\n  continue",
               "while True:\n    x = (y + 1i)\n    while False:\n        continue\n");
        }
    }

    SECTION("If statements") {
        st("if True:\n 1", "if True:\n    1i\n");
        st("if True:\n 1\nelse:\n 2", "if True:\n    1i\nelse:\n    2i\n");
        st("if True:\n 1\nelif False:\n 3\nelse:\n 2",
           "if True:\n    1i\nelif False:\n    3i\nelse:\n    2i\n");
    }

    SECTION("Def statements") {
        st(
                "def func():\n"
                "    y = x + 1\n"
                "    z = 9 + 2\n",
                "def func():\n"
                "    y = (x + 1i)\n"
                "    z = (9i + 2i)\n");
    }

    SECTION("Mixed statements") {
        st("print(5)\nprint(-6)", "print(5i)\nprint(-6i)\n");
    }
}
