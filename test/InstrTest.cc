#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;
using llvm::cast;

TEST_CASE("Instr") {
    SECTION("llvmPy_add: adding two ints will return an int") {
        PyInt lhs(1);
        PyInt rhs(2);
        PyObj *rv = llvmPy_add(lhs, rhs);
        CHECK(rv->as<PyInt>().getValue() == 3);
    }

    SECTION("llvmPy_int: return the integer representation") {
        PyInt i1(2), i2(-4);
        PyStr s1("3"), s2("-5"), s3("a");

        CHECK(llvmPy_int(i1)->getValue() == 2);
        CHECK(llvmPy_int(i2)->getValue() == -4);
        CHECK(llvmPy_int(s1)->getValue() == 3);
        CHECK(llvmPy_int(s2)->getValue() == -5);

        // It should throw an exception, but that hasn't been implemented.
        // CHECK(llvmPy_int(s3));
    }

    SECTION("llvmPy_str: return the string representation") {
        PyInt i1(2), i2(-4);
        PyBool b1(true);
        PyNone none;

        CHECK(llvmPy_str(i1)->getValue() == "2");
        CHECK(llvmPy_str(i2)->getValue() == "-4");
        CHECK(llvmPy_str(b1)->getValue() == "True");
        CHECK(llvmPy_str(none)->getValue() == "None");
    }

    SECTION("llvmPy_none: will return the singleton None instance") {
        PyObj *rv1 = llvmPy_none();
        PyObj *rv2 = llvmPy_none();
        CHECK(rv1->isa<PyNone>());
        CHECK(rv1 == rv2);
    }

    SECTION("llvmPy_func: will store the frame and function pointers") {
        auto frame = reinterpret_cast<FrameN*>(123);
        auto label = reinterpret_cast<void *>(888);
        PyFunc *pyfunc = llvmPy_func(frame, label);
        CHECK(pyfunc->getFrame() == frame);
        CHECK(pyfunc->getLabel() == label);
    }

    SECTION("llvmPy_fchk: will return the LLVM function and frame pointers") {
        auto frame = reinterpret_cast<FrameN*>(123);
        auto label = reinterpret_cast<void *>(888);

        PyFunc f(frame, label);
        FrameN *callframe;
        void *rv = llvmPy_fchk(&callframe, f, 0);
        CHECK(rv == label);
        CHECK(callframe == frame);
    }

    SECTION("llvmPy_truthy: will return the truth value of an object") {
        CHECK(llvmPy_truthy(llvmPy_True) == 1);
        CHECK(llvmPy_truthy(llvmPy_False) == 0);
        CHECK(llvmPy_truthy(llvmPy_None) == 0);

        PyStr a(""), b("test");

        CHECK(llvmPy_truthy(a) == 0);
        CHECK(llvmPy_truthy(b) == 1);
    }

    SECTION("llvmPy_bool: will return the __bool__ of the object") {
        auto const *t = &PyBool::get(true);
        auto const *f = &PyBool::get(false);

        CHECK(llvmPy_bool(llvmPy_True) == t);
        CHECK(llvmPy_bool(llvmPy_False) == f);
        CHECK(llvmPy_bool(llvmPy_None) == f);

        PyInt i1(-1), i2(0), i3(1), i4(30);

        CHECK(llvmPy_bool(i1) == t);
        CHECK(llvmPy_bool(i2) == f);
        CHECK(llvmPy_bool(i3) == t);
        CHECK(llvmPy_bool(i4) == t);

        PyStr s1(""), s2("test");

        CHECK(llvmPy_bool(s1) == f);
        CHECK(llvmPy_bool(s2) == t);
    }

    SECTION("llvmPy_len: will return the __len__ of the object") {
        PyStr s1(""), s2("test");
        CHECK(llvmPy_len(s1)->as<PyInt>().getValue() == 0);
        CHECK(llvmPy_len(s2)->as<PyInt>().getValue() == 4);
    }
}
