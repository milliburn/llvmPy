#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvm/Support/Casting.h>
#include <catch2/catch.hpp>
using namespace llvmPy;
using llvm::cast;

TEST_CASE("Instr", "[Instr]") {
    RT rt;
    Compiler compiler(rt);
    Emitter em(compiler);
    RTModule *mod = em.createModule("", {});

    SECTION("llvmPy_add: adding two ints will return an int") {
        PyInt lhs(1);
        PyInt rhs(2);
        PyObj *rv = llvmPy_add(lhs, rhs);
        REQUIRE(rv->isType(PyObjType::Int));
        CHECK(cast<PyInt>(rv)->getValue() == 3);
    }

    SECTION("llvmPy_int: will return an int") {
        PyObj *rv = llvmPy_int(5);
        REQUIRE(rv->isType(PyObjType::Int));
        CHECK(cast<PyInt>(rv)->getValue() == 5);
    }

    SECTION("llvmPy_none: will return the singleton None instance") {
        PyObj *rv1 = llvmPy_none();
        PyObj *rv2 = llvmPy_none();
        CHECK(rv1->isType(PyObjType::None));
        CHECK(rv1 == rv2);
    }

    SECTION("llvmPy_func: will store the frame and function pointers") {
        auto frame = reinterpret_cast<FrameN *>(123);
        auto rtfunc = reinterpret_cast<RTFunc *>(567);

        llvm::Function *function =
                llvm::Function::Create(
                        llvm::FunctionType::get(
                                llvm::Type::getVoidTy(compiler.getContext()),
                                {},
                                false),
                        llvm::Function::ExternalLinkage);

        function->setPrefixData(llvm::ConstantInt::get(
                llvm::Type::getInt64Ty(compiler.getContext()),
                reinterpret_cast<uint64_t>(rtfunc)));

        PyFunc *pyfunc = llvmPy_func(frame, function);

        CHECK(&pyfunc->getFrame() == frame);
        CHECK(&pyfunc->getFunc() == rtfunc);
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

        RTFunc rtf(*function, mod->getScope());
        PyFunc f(&rtf, frame);
        FrameN *callframe;
        llvm::Function *rv = llvmPy_fchk(&callframe, f, 0);
        CHECK(rv == function);
        CHECK(callframe == frame);
    }
}
