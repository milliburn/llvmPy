#include <llvmPy.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvm/Support/Casting.h>
#include <catch2/catch.hpp>
using namespace llvmPy;
using llvm::cast;

TEST_CASE("Instr", "[Instr]") {
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

    SECTION("llvmPy_fchk: will return the LLVM function pointer") {
        llvm::LLVMContext ctx;

        llvm::Function *function =
                llvm::Function::Create(
                        llvm::FunctionType::get(
                                llvm::Type::getVoidTy(ctx),
                                {},
                                false),
                        llvm::Function::ExternalLinkage);

        RTScope scope;
        RTFunc rtf(function, &scope);
        PyFunc f(rtf);
        llvm::Function *rv = llvmPy_fchk(f, 0);
        CHECK(rv == function);
    }
}
