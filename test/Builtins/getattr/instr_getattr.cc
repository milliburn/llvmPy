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

    SECTION("it will pass through any non-function") {
        PyStr name("func");
        Mock<PyObj> obj;
        When(Method(obj, py__getattr__)).Return(&name);
        PyObj *result = llvmPy_getattr(obj.get(), name);
        CHECK(result == &name);
    }

    SECTION("it can optionally create a bound function") {
        // The simulated syntax is `obj.func`.
        PyStr name("func");
        Mock<PyFunc> obj;
        auto * const label = reinterpret_cast<void *>(123);
        auto * const frame = reinterpret_cast<Frame *>(456);

        SECTION("library function attribute: no binding") {
            PyFunc *func = PyFunc::newLibraryFunction(label);
            When(Method(obj, py__getattr__)).Return(func);
            When(Method(obj, isInstance)).Return(false);
            PyFunc &result = llvmPy_getattr(obj.get(), name)->as<PyFunc>();
            CHECK(&result == func);
        }

        SECTION("library method attribute: bind self") {
            PyFunc *func = PyFunc::newLibraryFunction(label);
            When(Method(obj, py__getattr__)).Return(func);
            When(Method(obj, isInstance)).Return(true);
            auto &result = llvmPy_getattr(obj.get(), name)->as<PyFunc>();
            CHECK(&result != func);
            CHECK(result.getType() == PyFuncType::LibraryMethod);
            CHECK(result.isBound());
            CHECK(result.getCallFrame().self == &obj.get());
            CHECK(result.getCallFrame().label == label);
            CHECK(result.getCallFrame().frame == nullptr);
        }

        SECTION("user function attribute: no binding") {
            PyFunc *func = PyFunc::newUserFunction(label, frame);
            When(Method(obj, py__getattr__)).Return(func);
            When(Method(obj, isInstance)).Return(false);
            auto &result = llvmPy_getattr(obj.get(), name)->as<PyFunc>();
            CHECK(&result == func);
        }

        SECTION("user method attribute: bind self") {
            PyFunc *func = PyFunc::newUserFunction(label, frame);
            When(Method(obj, py__getattr__)).Return(func);
            When(Method(obj, isInstance)).Return(true);
            auto &result = llvmPy_getattr(obj.get(), name)->as<PyFunc>();
            CHECK(&result != func);
            CHECK(result.getType() == PyFuncType::UserMethod);
            CHECK(result.isBound());
            CHECK(result.getCallFrame().self == &obj.get());
            CHECK(result.getCallFrame().label == label);
            CHECK(result.getCallFrame().frame == frame);
        }


        Verify(Method(obj, py__getattr__)).Once();
        Verify(Method(obj, isInstance)).Once();
        VerifyNoOtherInvocations(obj);
    }
}
