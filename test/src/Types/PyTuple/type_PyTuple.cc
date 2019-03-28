#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyTuple.h>
#include <llvmPy/Test/PyObj_DSL.h>
#include <catch2.h>
#include <string>
using namespace llvmPy;

static PyTuple &
tup()
{
    return PyTuple::get(0, nullptr);
}

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

    SECTION("py__len__()") {
        CHECK(t0.py__len__() == 0);
        CHECK(t1.py__len__() == 1);
        CHECK(t2.py__len__() == 2);
        CHECK(t3.py__len__() == 3);
    }

    SECTION("py__eq__()") {
        CHECK(t0 == t0);
        CHECK(!(t0 == t1));
        CHECK(t1 == t1);
        CHECK(!(t1 == t2));
    }

    SECTION("py__lt__()") {
        // Empty tuple is less than non-empty tuple.
        // Otherwise compare elements lexicographically.
        CHECK(!(tup() < tup()));
        CHECK(tup() < tup(1));
        CHECK(tup() < tup(1, 2));
        CHECK(!(tup(1) < tup()));
        CHECK(!(tup(1) < tup(1, 2)));
        CHECK(tup(1) < tup(2));
        CHECK(tup(1) < tup(2, 1));
        CHECK(tup(2, 1) < tup(2, 2));
        CHECK(tup(2, 1, 3) < tup(2, 2));
        CHECK(tup(1, 2, 3) < tup(2));
        CHECK(!(tup(2, 3) < tup(2)));
    }

    SECTION("py__le__()") {
        CHECK(tup() <= tup());
        CHECK(tup() <= tup(1));
        CHECK(!(tup(1) <= tup()));
    }

    SECTION("py__ge__()") {
        CHECK(tup() >= tup());
        CHECK(!(tup() >= tup(1)));
        CHECK(tup(1) >= tup());
    }

    SECTION("py__gt__()") {
        CHECK(!(tup() > tup()));
        CHECK(!(tup() > tup(1)));
        CHECK(tup(1) > tup());
    }

    SECTION("py__getitem__()") {
        CHECK(t3.py__getitem__(*new PyInt(0)) == values[0]);
        CHECK(t3.py__getitem__(*new PyInt(-3)) == values[0]);
        CHECK(t3.py__getitem__(*new PyInt(1)) == values[1]);
        CHECK(t3.py__getitem__(*new PyInt(-2)) == values[1]);
        CHECK(t3.py__getitem__(*new PyInt(2)) == values[2]);
        CHECK(t3.py__getitem__(*new PyInt(-1)) == values[2]);
    }
}
