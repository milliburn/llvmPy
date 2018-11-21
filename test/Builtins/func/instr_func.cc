#include <llvmPy/Instr.h>
#include <llvmPy/RT/Scope.h>
#include <catch2/catch.hpp>
#include <fakeit.hpp>
using namespace llvmPy;
using namespace fakeit;

static Frame *
allocFrame(size_t slotCount) {
    size_t size = sizeof(Frame) + slotCount * sizeof(PyObj *);
    Frame *frame = reinterpret_cast<Frame *>(calloc(1, size));
    frame->self = frame;
    return frame;
}

TEST_CASE("instr: llvmPy_func()", "[instr][func]") {
    Mock<Scope> parentScope;
    Mock<Scope> scope;

    When(Method(scope, hasParent)).AlwaysReturn(true);
    When(Method(scope, getParent)).AlwaysReturn(parentScope.get());

    SECTION("it stores the frame and function pointers") {
        Frame frame;
        frame.self = reinterpret_cast<Frame *>(123);
        frame.setIsPointingToHeap();
        void *labelData[2] = {
                reinterpret_cast<void *>(&scope.get()),
                nullptr
        };
        auto *label = &labelData[1];

        PyFunc *pyfunc = llvmPy_func(&frame, label);
        Verify(Method(scope, getParent)).Once();
        VerifyNoOtherInvocations(scope);
        VerifyNoOtherInvocations(parentScope);
        CHECK(pyfunc->getFrame() == reinterpret_cast<Frame *>(123));
        CHECK(pyfunc->getLabel() == label);
    }

    SECTION("the allocFrame() test helper nulls and self-initialises a frame") {
        // allocFrame() is used to simplify the tests that follow.
        Frame *frame = allocFrame(3);
        CHECK(frame->self == frame);
        CHECK(frame->outer == nullptr);
        for (int i = 0; i < 3; ++i) {
            CHECK(frame->vars[i] == nullptr);
        }
    }

    SECTION("moveFrameToHeap()") {
        PyInt i1(1), i2(2), i3(3), i4(4);

        SECTION("it should return null and do nothing if the frame is null") {
            auto *actual = moveFrameToHeap(nullptr, scope.get());
            CHECK(actual == nullptr);
        }

        SECTION("it should return the same frame if it's already relocated") {
            Frame frame;
            frame.self = reinterpret_cast<Frame *>(123);
            frame.setIsPointingToHeap();
            auto *actual = moveFrameToHeap(&frame, scope.get());
            CHECK(actual == reinterpret_cast<Frame *>(123));
        }

        SECTION("it should relocate a 1-deep frame") {
            size_t const slotCount = 2;
            Mock<Scope> scope;
            When(Method(scope, hasParent)).AlwaysReturn(false);
            When(Method(scope, getSlotCount)).AlwaysReturn(slotCount);

            Frame *in = allocFrame(slotCount);

            CHECK(!in->isPointingToHeap());

            in->vars[0] = &i1;
            in->vars[1] = &i2;

            Frame *out = moveFrameToHeap(in, scope.get());

            // The stack frame's self-pointer should now point at the heap.

            CHECK(out != in);
            CHECK(in->self == out);
            CHECK(in->isPointingToHeap());
            CHECK(out->self == nullptr);
            CHECK(out->outer == nullptr);
            CHECK(in->vars[0] == nullptr);
            CHECK(out->vars[0] == &i1);
            CHECK(in->vars[1] == nullptr);
            CHECK(out->vars[1] == &i2);

            Verify(Method(scope, getSlotCount)).Once();
            Verify(Method(scope, hasParent)).Once();
            VerifyNoOtherInvocations(scope);
        }

        SECTION("it should not attempt to relocate a frame twice") {
            Mock<Scope> scope;
            When(Method(scope, hasParent)).AlwaysReturn(false);
            When(Method(scope, getSlotCount)).AlwaysReturn(1);

            Frame *frame = allocFrame(1);
            frame->vars[0] = &i1;

            Frame *out1 = moveFrameToHeap(frame, scope.get());
            Frame *out2 = moveFrameToHeap(frame, scope.get());

            CHECK(frame->self == out1);
            CHECK(out1->self == nullptr);
            CHECK(out1 == out2);

            Verify(Method(scope, getSlotCount)).Once();
            Verify(Method(scope, hasParent)).Once();
            VerifyNoOtherInvocations(scope);
        }

        SECTION("it should relocate a 3-deep frame") {
            Mock<Scope> mock1;
            Mock<Scope> mock2;
            Mock<Scope> mock3;

            Frame *f1s = allocFrame(1);
            Frame *f2s = allocFrame(1);
            Frame *f3s = allocFrame(1);

            f1s->vars[0] = &i1;
            When(Method(mock1, getSlotCount)).AlwaysReturn(1);
            When(Method(mock1, hasParent)).AlwaysReturn(false);

            f2s->outer = f1s;
            f2s->vars[0] = &i2;
            When(Method(mock2, getSlotCount)).AlwaysReturn(1);
            When(Method(mock2, hasParent)).AlwaysReturn(true);
            When(Method(mock2, getParent)).AlwaysReturn(mock1.get());

            f3s->outer = f2s;
            f3s->vars[0] = &i3;
            When(Method(mock3, getSlotCount)).AlwaysReturn(1);
            When(Method(mock3, hasParent)).AlwaysReturn(true);
            When(Method(mock3, getParent)).AlwaysReturn(mock2.get());

            SECTION("entirely if all on stack") {
                Frame *f3h = moveFrameToHeap(f3s, mock3.get());

                CHECK(f3s->isPointingToHeap());
                CHECK(f2s->isPointingToHeap());
                CHECK(f1s->isPointingToHeap());

                CHECK(f3s->self == f3h);
                CHECK(f3h != f3s);
                CHECK(f3h->self == nullptr);
                CHECK(f3h->isOnHeap());
                CHECK(f3h->vars[0] == &i3);

                f3h = f3h->outer;

                CHECK(f3h != f2s);
                CHECK(f3h->self == nullptr);
                CHECK(f3h->isOnHeap());
                CHECK(f3h->vars[0] == &i2);

                f3h = f3h->outer;

                CHECK(f3h != f1s);
                CHECK(f3h->self == nullptr);
                CHECK(f3h->outer == nullptr);
                CHECK(f3h->isOnHeap());
                CHECK(f3h->vars[0] == &i1);
            }

            SECTION("partially if one has already been relocated") {
                f2s->setIsPointingToHeap();
                Frame *f3h = moveFrameToHeap(f3s, mock3.get());

                // Explicitly test that the frame copy did not extend
                // beyond the first heap-allocated frame.

                CHECK(f3h != f3s);
                CHECK(f3s->self == f3h);
                CHECK(f3s->isPointingToHeap());
                CHECK(f3h->outer == f2s);
                CHECK(f3h->vars[0] == &i3);
                CHECK(f1s->self == f1s);
            }
        }
    }
}
