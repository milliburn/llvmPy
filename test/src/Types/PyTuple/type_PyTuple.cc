#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyTuple.h>
#include <catch2.h>
#include <string>
using namespace llvmPy;

static PyTuple &
tup(int64_t value)
{
    PyObj *values[] = { new PyInt(value) };
    return PyTuple::get(1, values);
}

static PyTuple &
tup(int64_t value1, int64_t value2)
{
    PyObj *values[] = { new PyInt(value1), new PyInt(value2) };
    return PyTuple::get(2, values);
}

static PyTuple &
tup(int64_t value1, int64_t value2, int64_t value3)
{
    PyObj *values[] = { new PyInt(value1), new PyInt(value2), new PyInt(value3) };
    return PyTuple::get(3, values);
}

TEST_CASE("type: PyTuple", "[types][PyTuple]") {
    PyObj *values[] = {
            new PyInt(1),
            new PyInt(2),
            new PyInt(3),
    };

    PyTuple &t0 = PyTuple::get(0, nullptr);
    PyTuple &t1 = PyTuple::get(1, values);
    PyTuple &t2 = PyTuple::get(2, values);
    PyTuple &t3 = PyTuple::get(3, values);

    // PyTuple *all[] = { &t0, &t1, &t2, &t3 };

    SECTION("empty tuple is singleton") {
        PyTuple &empty = PyTuple::get(0, nullptr);
        CHECK(&t0 == &empty);
    }

    SECTION("py__str__()") {
        CHECK(t0.py__str__() == "()");
        CHECK(t1.py__str__() == "(1,)");
        CHECK(t2.py__str__() == "(1, 2)");
        CHECK(t3.py__str__() == "(1, 2, 3)");
    }

    SECTION("py__bool__()") {
        CHECK(!t0.py__bool__());
        CHECK(t1.py__bool__());
        CHECK(t2.py__bool__());
        CHECK(t3.py__bool__());
    }

    SECTION("py__eq__()") {
        CHECK(t0.py__eq__(t0));
        CHECK(!t0.py__eq__(t1));
        CHECK(t1.py__eq__(t1));
        CHECK(!t1.py__eq__(t2));
    }

    SECTION("py__lt__()") {
        CHECK(!t0.py__lt__(t0));
        CHECK(t0.py__lt__(t1));
        CHECK(!t1.py__lt__(t0));
        CHECK(tup(1).py__lt__(tup(2)));
        CHECK(tup(1).py__lt__(tup(1, 2)));
        CHECK(tup(1).py__lt__(tup(2, 1)));
        CHECK(tup(1).py__lt__(tup(1, 2, 3)));
        CHECK(tup(1).py__lt__(tup(3, 2, 1)));
        CHECK(!tup(2).py__lt__(tup(1)));
    }
}
