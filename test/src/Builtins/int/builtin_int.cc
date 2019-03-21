#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("builtin: int()", "[builtins][int]") {
    PyInt i1(2), i2(-4);
    PyStr s1("3"), s2("-5"), s3("a");

    SECTION("it returns the contents of a PyInt") {
        CHECK(llvmPy_int(i1)->getValue() == 2);
        CHECK(llvmPy_int(i2)->getValue() == -4);
    }

    SECTION("it returns the integer conversion of a PyStr") {
        CHECK(llvmPy_int(s1)->getValue() == 3);
        CHECK(llvmPy_int(s2)->getValue() == -5);
    }

    // It should throw an exception, but that hasn't been implemented.
    // CHECK(llvmPy_int(s3));
}
