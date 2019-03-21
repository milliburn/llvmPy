#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("builtin: binary add", "[builtins][add]") {
    PyInt lhs(1);
    PyInt rhs(2);
    PyObj *rv = llvmPy_add(lhs, rhs);
    CHECK(rv->as<PyInt>().getValue() == 3);
}
