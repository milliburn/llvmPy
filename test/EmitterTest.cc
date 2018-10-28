#include <llvmPy.h>
#include <catch2/catch.hpp>
#include "testutil.h"
#include <string>
using namespace llvmPy;
using std::string;
using std::to_string;

static void
test(string section, string subsection, string description)
{
    SECTION(description) {
        string actual = emitFile(
                "EmitterTest/" +
                section +
                "/" +
                subsection + ".py");

        string expect = readFile(
                "EmitterTest/" +
                section +
                "/" +
                subsection + ".ll");

        REQUIRE(actual == expect);
    }
}

static void
test(string section, string subsection)
{
    test(section, subsection, section + " " + subsection);
}

TEST_CASE("Emitter", "[Emitter]") {
    test("Literals", "Int.1", "Integer literal");
    test("Literals", "None", "None literal");
    test("Statement", "Assign.1", "Assign statement");
    test("BinOp", "Add.1", "Binary addition operator");
    // test("Call", "Call.0", "Function call with no arguments");
}
