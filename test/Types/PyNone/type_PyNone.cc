#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyNone.h>
#include <catch2.h>
using namespace llvmPy;

TEST_CASE("type: PyNone", "[types][PyNone]") {
    PyNone &a = PyNone::get();
    PyNone &b = PyNone::get();

    SECTION("PyNone is singleton") {
        CHECK(&a == &b);
    }

    SECTION("py__bool__()") {
        CHECK(!PyNone::get().py__bool__());
    }
}
