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

    SECTION("it relocates the frame from the stack to the heap") {

    }

    SECTION("moveFrameToHeap()") {
        PyInt i1(1), i2(2), i3(3), i4(4);

        SECTION("it should return null and do nothing if the frame is null") {
            auto *actual = moveFrameToHeap(nullptr, scope.get());
            CHECK(actual == nullptr);
        }

        SECTION("it should return the same frame if its self-pointer is null") {
            auto *actual = moveFrameToHeap(&frame, scope.get());
            CHECK(actual == &frame);
        }

        SECTION("it should relocate a frame that's 1-deep") {
            size_t const slotCount = 2;
            Frame *in = reinterpret_cast<Frame *>(malloc(
                    sizeof(Frame) + slotCount * sizeof(*in->vars)));

            in->self = in;
            in->outer = nullptr;
            in->vars[0] = &i1;
            in->vars[1] = &i2;
            When(Method(scope, getSlotCount)).Return(slotCount);

            Frame *out = moveFrameToHeap(in, scope.get());

            // The stack frame's self-pointer should now point at the heap.

            CHECK(out != in);
            CHECK(in->self == out);
            CHECK(out->self == nullptr); // Marker
            CHECK(out->outer == nullptr);
            CHECK(in->vars[0] == nullptr);
            CHECK(out->vars[0] == &i1);
            CHECK(in->vars[1] == nullptr);
            CHECK(out->vars[1] == &i2);

            Verify(Method(scope, getSlotCount)).Once();
            Verify(Method(scope, getParent)).Once();
            VerifyNoOtherInvocations(scope);
            VerifyNoOtherInvocations(parentScope);
        }
    }
}
