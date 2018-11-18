#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2/catch.hpp>
#include <fakeit.hpp>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_truthy()", "[instr][truthy]") {
    Mock<PyObj> mock;

    SECTION("it delegates to the object's py__bool__()") {
        When(Method(mock, py__bool__)).Return(true);
        uint8_t isTruthy = llvmPy_truthy(mock.get());
        Verify(Method(mock, py__bool__)).Once();
        VerifyNoOtherInvocations(mock);
        CHECK(isTruthy == 1);
    }

    SECTION("it returns 1 for true and 0 for false") {
        CHECK(llvmPy_truthy(PyBool::get(true)) == 1);
        CHECK(llvmPy_truthy(PyBool::get(false)) == 0);
    }

}
