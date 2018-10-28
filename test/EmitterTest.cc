#include <llvmPy.h>
#include <catch2/catch.hpp>
#include "testutil.h"
#include <string>
using namespace llvmPy;
using std::string;
using std::to_string;

static void
test(string section, int index, string description)
{
    SECTION(description) {
        string actual = emitFile(
                "EmitterTest/" +
                section +
                "/" +
                section +
                "." + to_string(index) + ".py");

        string expect = readFile(
                "EmitterTest/" +
                section +
                "/" +
                section +
                "." + to_string(index) + ".ll");

        REQUIRE(actual == expect);
    }
}

static void
test(string section, int index)
{
    test(section, index, section + " " + to_string(index));
}

TEST_CASE("Emitter", "[Emitter]") {
    test("IntLiterals", 1, "Integer literal");
    test("AssignStmt", 1, "Assign statement");
}
