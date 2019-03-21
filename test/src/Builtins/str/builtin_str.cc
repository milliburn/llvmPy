#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("builtin: str()", "[builtins][str]") {
    Mock<PyObj> mock;
    std::string const str("test");

    SECTION("it delegates to the object's py__str__()") {
        When(Method(mock, py__str__)).Return(str);
        PyStr const *rv = llvmPy_str(mock.get());
        Verify(Method(mock, py__str__)).Once();
        VerifyNoOtherInvocations(mock);
        CHECK(rv->getValue() == str);
    }

    SECTION("it returns an object's string representation") {
        PyInt i1(2), i2(-4);
        PyBool b1(true);
        PyNone none;

        CHECK(llvmPy_str(i1)->getValue() == "2");
        CHECK(llvmPy_str(i2)->getValue() == "-4");
        CHECK(llvmPy_str(b1)->getValue() == "True");
        CHECK(llvmPy_str(none)->getValue() == "None");
    }
}
