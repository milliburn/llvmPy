#include <llvmPy/Instr.h>
#include <llvmPy/RT/Scope.h>
#include <catch2/catch.hpp>
#include <fakeit.hpp>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_func()", "[instr][func]") {
    Mock<Scope> parentScope;
    Mock<Scope> scope;
    Frame frame = { .self = nullptr, .outer = nullptr };
    void *labelData[2] = { reinterpret_cast<void *>(&scope.get()), nullptr };
    auto *label = &labelData[1];

    When(Method(scope, getParent)).AlwaysReturn(parentScope.get());

    SECTION("it stores the frame and function pointers") {
        PyFunc *pyfunc = llvmPy_func(&frame, label);
        Verify(Method(scope, getParent)).Once();
        VerifyNoOtherInvocations(scope);
        VerifyNoOtherInvocations(parentScope);
        CHECK(pyfunc->getFrame() == &frame);
        CHECK(pyfunc->getLabel() == label);
    }
}
