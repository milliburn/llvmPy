#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyTuple.h>
#include <catch2.h>
using namespace llvmPy;

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
        CHECK( t1.py__bool__());
        CHECK( t2.py__bool__());
        CHECK( t3.py__bool__());
    }
}
