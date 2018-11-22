#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_getattr()", "[instr][getattr]") {
    SECTION("it delegates to the object's py__getattr__()") {
        PyStr attr("test");
        PyInt i1(1);
        Mock<PyObj> mock;
        When(Method(mock, py__getattr__).Using(attr.str())).Return(&i1);
        auto *result = llvmPy_getattr(mock.get(), attr);
        Verify(Method(mock, py__getattr__).Using(attr.str())).Once();
        VerifyNoOtherInvocations(mock);
        CHECK(result == &i1);
    }
}
