#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;

TEST_CASE("PyObj") {
    SECTION("PyBool") {
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

    SECTION("PyNone") {
        PyNone &a = PyNone::get();
        PyNone &b = PyNone::get();

        SECTION("PyNone is singleton") {
            CHECK(&a == &b);
        }

        SECTION("py__bool__()") {
            CHECK(!PyNone::get().py__bool__());
        }
    }

    SECTION("PyStr") {
        PyStr s1(""), s2("a"), s3("test");

        SECTION("py__bool__()") {
            CHECK(!s1.py__bool__());
            CHECK(s2.py__bool__());
        }

        SECTION("py__len__()") {
            CHECK(s1.py__len__() == 0);
            CHECK(s2.py__len__() == 1);
            CHECK(s3.py__len__() == 4);
        }

        SECTION("py__add__()") {
            CHECK(s1.py__add__(s1)->as<PyStr>().getValue() == "");
            CHECK(s1.py__add__(s2)->as<PyStr>().getValue() == "a");
            CHECK(s2.py__add__(s2)->as<PyStr>().getValue() == "aa");
            CHECK(s2.py__add__(s3)->as<PyStr>().getValue() == "atest");
        }

        SECTION("py__eq__()") {
            CHECK(s1.py__eq__(s1));
            CHECK(!s1.py__eq__(s2));
            CHECK(!s2.py__eq__(s1));

            // Ensure the comparison is by value, not reference.
            PyStr s21("a"), s22("a");
            CHECK(s21.py__eq__(s22));
        }
    }
}
