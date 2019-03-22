#include <llvmPy/AST.h>
#include <catch2.h>
using namespace llvmPy;

TEST_CASE("AST: CompoundStmt", "[AST][CompoundStmt]") {
    PassStmt pass1;
    PassStmt pass2;
    PassStmt pass3;
    CompoundStmt compound;

    SECTION("insertBefore()") {
        compound.addStatement(pass1);
        compound.addStatement(pass2);
        REQUIRE(compound.statementsCount() == 2);

        SECTION("it should insert at the beginning if marker is null") {
            compound.insertBefore(nullptr, pass3);
            REQUIRE(compound.statementsCount() == 3);
            CHECK(&compound.at(0) == &pass3);
            CHECK(&compound.at(1) == &pass1);
            CHECK(&compound.at(2) == &pass2);
        }

        SECTION("it should insert before the given marker") {
            compound.insertBefore(&pass2, pass3);
            REQUIRE(compound.statementsCount() == 3);
            CHECK(&compound.at(0) == &pass1);
            CHECK(&compound.at(1) == &pass3);
            CHECK(&compound.at(2) == &pass2);
        }
    }

    SECTION("insertAfter()") {
        compound.addStatement(pass1);
        compound.addStatement(pass2);
        REQUIRE(compound.statementsCount() == 2);

        SECTION("it should insert at the end if marker is null") {
            compound.insertAfter(nullptr, pass3);
            REQUIRE(compound.statementsCount() == 3);
            CHECK(&compound.at(0) == &pass1);
            CHECK(&compound.at(1) == &pass2);
            CHECK(&compound.at(2) == &pass3);
        }

        SECTION("it should insert after the given marker") {
            compound.insertAfter(&pass1, pass3);
            REQUIRE(compound.statementsCount() == 3);
            CHECK(&compound.at(0) == &pass1);
            CHECK(&compound.at(1) == &pass3);
            CHECK(&compound.at(2) == &pass2);
        }
    }
}
