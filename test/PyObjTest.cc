#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("PyObj") {
    SECTION("PyBool") {
        PyBool &a = PyBool::get(true);
        PyBool &b = PyBool::get(false);

        SECTION("Accessors match the constructor") {
            CHECK(a.getValue() == true);
            CHECK(b.getValue() == false);
        }

        SECTION("py__str__()") {
            CHECK(a.py__str__() == "True");
            CHECK(b.py__str__() == "False");
        }

        SECTION("Booleans are singleton") {
            CHECK(&PyBool::get(true) == &a);
            CHECK(&PyBool::get(false) == &b);
            CHECK(&PyBool::get(true) != &PyBool::get(false));
        }
    }
}
