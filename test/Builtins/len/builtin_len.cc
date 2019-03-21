#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("builtin: len()", "[builtins][len]") {
    SECTION("it will return the __len__ of an object") {
        PyStr s1(""), s2("test");
        CHECK(llvmPy_len(s1)->as<PyInt>().getValue() == 0);
        CHECK(llvmPy_len(s2)->as<PyInt>().getValue() == 4);
    }
}
