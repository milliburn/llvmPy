#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <catch2/catch.hpp>
using namespace llvmPy;
using llvm::cast;

TEST_CASE("Instr", "[Instr]") {
    Compiler compiler;
    Emitter em(compiler);
    RTModule *mod = em.createModule("");

    SECTION("llvmPy_add: adding two ints will return an int") {
        PyInt lhs(1);
        PyInt rhs(2);
        PyObj *rv = llvmPy_add(lhs, rhs);
        CHECK(rv->as<PyInt>().getValue() == 3);
    }

    SECTION("llvmPy_int: will return an int") {
        PyObj *rv = llvmPy_int(5);
        CHECK(rv->as<PyInt>().getValue() == 5);
    }

    SECTION("llvmPy_none: will return the singleton None instance") {
        PyObj *rv1 = llvmPy_none();
        PyObj *rv2 = llvmPy_none();
        CHECK(rv1->isa<PyNone>());
        CHECK(rv1 == rv2);
    }

    SECTION("llvmPy_func: will store the frame and function pointers") {
        auto frame = reinterpret_cast<FrameN *>(123);
        auto rtfunc = reinterpret_cast<RTFunc *>(567);
        uint64_t labelData[2] = { reinterpret_cast<uint64_t>(rtfunc), 999 };
        void *label = &labelData[1];

        PyFunc *pyfunc = llvmPy_func(frame, label);

        CHECK(&pyfunc->getFrame() == frame);
        CHECK(&pyfunc->getFunc() == rtfunc);
        CHECK(pyfunc->getLabel() == label);
    }

    SECTION("llvmPy_fchk: will return the LLVM function and frame pointers") {
        llvm::Function *function =
                llvm::Function::Create(
                        llvm::FunctionType::get(
                                llvm::Type::getVoidTy(compiler.getContext()),
                                {},
                                false),
                        llvm::Function::ExternalLinkage);

        auto frame = reinterpret_cast<FrameN*>(123);
        void *label = reinterpret_cast<void *>(888);

        RTFunc rtf(*function, mod->getScope());
        PyFunc f(&rtf, frame, label);
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
}
