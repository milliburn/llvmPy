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

    SECTION("PyInt") {
        PyInt one(1);
        PyInt two1(2);
        PyInt two2(2);
        PyInt three(3);

        SECTION("Accessor matches the constructor") {
            CHECK(one.getValue() == 1);
            CHECK(two1.getValue() == 2);
        }

        SECTION("py__str__()") {
            CHECK(one.py__str__() == "1");
            CHECK(two1.py__str__() == "2");
        }

        SECTION("py__lt__()") {
            CHECK(!one.py__lt__(one));
            CHECK(one.py__lt__(two1));
            CHECK(one.py__lt__(two2));
            CHECK(!two1.py__lt__(two2));
            CHECK(one.py__lt__(three));
        }

        SECTION("py__le__()") {
            CHECK(one.py__le__(one));
            CHECK(one.py__le__(two1));
            CHECK(two1.py__le__(two2));
            CHECK(one.py__le__(two2));
            CHECK(one.py__le__(three));
        }

        SECTION("py__eq__()") {
            CHECK(one.py__eq__(one));
            CHECK(!one.py__eq__(two1));
            CHECK(two1.py__eq__(two2));
            CHECK(!two1.py__eq__(three));
        }

        SECTION("py__ge__()") {
            CHECK(one.py__ge__(one));
            CHECK(!one.py__ge__(two1));
            CHECK(!one.py__ge__(two2));
            CHECK(two1.py__ge__(two2));
            CHECK(!one.py__ge__(three));
            CHECK(two1.py__ge__(one));
        }

        SECTION("py__gt__()") {
            CHECK(!one.py__gt__(one));
            CHECK(!one.py__gt__(two1));
            CHECK(!one.py__gt__(two2));
            CHECK(!two1.py__gt__(two2));
            CHECK(!one.py__gt__(three));
            CHECK(three.py__gt__(one));
        }
    }
}
