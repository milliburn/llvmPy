#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
#include <array>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("types: PyObj", "[types][PyObj]") {
    PyObj obj1, obj2;
    std::array<bool, 2> bools = { true, false };

    SECTION("py__eq__()") {
        CHECK(obj1.py__eq__(obj1));
        CHECK(!obj1.py__eq__(obj2));
    }

    SECTION("py__ne__() is the negation of py__eq__()") {
        for (auto v : bools) {
            Mock<PyObj> spy(obj1);
            Spy(Method(spy, py__eq__));
            When(Method(spy, py__eq__)).Return(v);
            bool rv = spy.get().py__ne__(obj1);
            Verify(Method(spy, py__eq__)).Once();
            VerifyNoOtherInvocations(spy);
            CHECK(rv != v);
        }
    }

    SECTION("py__le__() is the negation of py__gt__()") {
        for (auto v : bools) {
            Mock<PyObj> spy(obj1);
            Spy(Method(spy, py__gt__));
            When(Method(spy, py__gt__)).Return(v);
            bool rv = spy.get().py__le__(obj1);
            Verify(Method(spy, py__gt__)).Once();
            VerifyNoOtherInvocations(spy);
            CHECK(rv != v);
        }
    }

    SECTION("py__ge__() is the negation of py__lt__()") {
        for (auto v : bools) {
            Mock<PyObj> spy(obj1);
            Spy(Method(spy, py__lt__));
            When(Method(spy, py__lt__)).Return(v);
            bool rv = spy.get().py__ge__(obj1);
            Verify(Method(spy, py__lt__)).Once();
            VerifyNoOtherInvocations(spy);
            CHECK(rv != v);
        }
    }
}
