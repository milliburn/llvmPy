#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_getitem()", "[instr][getitem]") {
    SECTION("it delegates to the object's py__getitem__()") {
        PyObj key, rv;
        Mock<PyObj> mock;
        When(Method(mock, py__getitem__)).Return(&rv);
        auto *result = llvmPy_getitem(mock.get(), key);
        Verify(Method(mock, py__getitem__)).Once();
        VerifyNoOtherInvocations(mock);
        CHECK(result == &rv);
    }
}
