#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("builtin: none", "[builtins][none]") {
    SECTION("it will return the singleton None instance") {
        PyObj *rv1 = llvmPy_none();
        PyObj *rv2 = llvmPy_none();
        CHECK(rv1->isa<PyNone>());
        CHECK(rv1 == rv2);
    }
}
