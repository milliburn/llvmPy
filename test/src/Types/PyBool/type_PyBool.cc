#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyBool.h>
#include <catch2.h>
using namespace llvmPy;

TEST_CASE("type: PyBool", "[types][PyBool]") {
    PyBool &t = PyBool::get(true);
    PyBool &f = PyBool::get(false);

    SECTION("Accessors match the constructor") {
        CHECK(t.getValue() == true);
        CHECK(f.getValue() == false);
    }

    SECTION("py__str__()") {
        CHECK(t.py__str__() == "True");
        CHECK(f.py__str__() == "False");
    }

    SECTION("py__bool__()") {
        CHECK(t.py__bool__() == true);
        CHECK(f.py__bool__() == false);
    }

    SECTION("Booleans are singleton") {
        CHECK(&PyBool::get(true) == &t);
        CHECK(&PyBool::get(false) == &f);
        CHECK(&PyBool::get(true) != &PyBool::get(false));
    }
}
