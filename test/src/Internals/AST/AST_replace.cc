#include <llvmPy/AST.h>
#include <catch2.h>
using namespace llvmPy;

#define RUN_TEST(instance, method) \
    REQUIRE(oldval.hasParent()); \
    REQUIRE(&instance.method == &oldval); \
    REQUIRE(instance.replace(irrelevant, newval) == nullptr); \
    REQUIRE(&instance.method == &oldval); \
    REQUIRE(instance.replace(oldval, newval) == &oldval); \
    REQUIRE(&instance.method == &newval); \
    REQUIRE(!oldval.hasParent()); \
    do {} while(0)

TEST_CASE("AST: replace() operation", "[AST][replace]") {
    SECTION("Expressions") {
        IntegerExpr oldval(1);
        IntegerExpr oldval2(1);
        IntegerExpr newval(2);
        IntegerExpr irrelevant(3);

        SECTION("Unary") {
            UnaryExpr expr(TokenType::tok_not, oldval);
            RUN_TEST(expr, getExpr());
        }

        SECTION("Binary") {
            SECTION("left operand") {
                BinaryExpr expr(oldval, TokenType::tok_add, oldval2);
                RUN_TEST(expr, getLeftOperand());
            }

            SECTION("right operand") {
                BinaryExpr expr(oldval2, TokenType::tok_add, oldval);
                RUN_TEST(expr, getRightOperand());
            }
        }

        SECTION("Tuple") {
            TupleExpr expr;

            SECTION("first member") {
                expr.addMember(oldval);
                expr.addMember(oldval2);
                RUN_TEST(expr, at(0));
            }

            SECTION("second member") {
                expr.addMember(oldval2);
                expr.addMember(oldval);
                RUN_TEST(expr, at(1));
            }
        }
    }

    SECTION("Statements") {
        PassStmt oldval;
        PassStmt oldval2;
        PassStmt newval;
        PassStmt irrelevant;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
        SECTION("Expr") {
            IntegerExpr oldval(1);
            IntegerExpr newval(2);
            IntegerExpr irrelevant(3);
            ExprStmt stmt(oldval);
            RUN_TEST(stmt, getExpr());
        }
#pragma GCC diagnostic pop

        SECTION("Compound") {
            CompoundStmt stmt;
            stmt.addStatement(oldval);
            stmt.addStatement(oldval2);
            RUN_TEST(stmt, at(0));
        }
    }
}
