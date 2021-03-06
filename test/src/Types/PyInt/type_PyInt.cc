#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyInt.h>
#include <catch2.h>
using namespace llvmPy;

TEST_CASE("type: PyInt", "[types][PyInt]") {
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

    SECTION("py__bool__()") {
        CHECK(PyInt(-1).py__bool__());
        CHECK(!PyInt(0).py__bool__());
        CHECK(PyInt(1).py__bool__());
        CHECK(PyInt(30).py__bool__());
    }

    SECTION("py__add__()") {
        PyInt l(2), r(3);
        PyObj *rv = l.py__add__(r);
        CHECK(rv->as<PyInt>().getValue() == 5);
    }

    SECTION("py__sub__()") {
        PyInt l(2), r(3);
        PyObj *rv = l.py__sub__(r);
        CHECK(rv->as<PyInt>().getValue() == -1);
    }

    SECTION("py__mul__()") {
        PyInt l(2), r(3);
        PyObj *rv = l.py__mul__(r);
        CHECK(rv->as<PyInt>().getValue() == 6);
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
