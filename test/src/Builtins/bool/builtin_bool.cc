#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("builtin: bool()", "[builtins][bool]") {
    Mock<PyObj> mock;

    SECTION("it delegates to the object's py__bool__()") {
        When(Method(mock, py__bool__)).Return(false);
        PyBool *rv = llvmPy_bool(mock.get());
        Verify(Method(mock, py__bool__)).Once();
        VerifyNoOtherInvocations(mock);
        CHECK(!rv->getValue());
    }
}
