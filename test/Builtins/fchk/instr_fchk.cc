#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_fchk()", "[instr][fchk]") {
    auto * const label = reinterpret_cast<void *>(123);
    auto * const frame = reinterpret_cast<Frame *>(456);
    Frame *callframe = nullptr;

    SECTION("it will set the call frame in accordance with the function") {
        Mock<PyObj> obj;

        SECTION("library function: callframe is set to null") {
            callframe = reinterpret_cast<Frame *>(987);
            PyFunc func = PyFunc::createLibraryFunction(label);
            void *result = llvmPy_fchk(&callframe, func, 0);
            CHECK(callframe == nullptr);
            CHECK(result == func.getLabel());
        }

        SECTION("library method: callframe is the self pointer") {
            PyFunc *func = PyFunc::newLibraryMethod(label, &obj.get());
            void *result = llvmPy_fchk(&callframe, *func, 0);
            CHECK(reinterpret_cast<PyObj *>(callframe) == &obj.get());
            CHECK(result == label);
        }

        SECTION("user function: callframe is the frame pointer") {
            PyFunc func = PyFunc::createUserFunction(label, frame);
            void *result = llvmPy_fchk(&callframe, func, 0);
            CHECK(callframe == frame);
            CHECK(result == label);
        }

        SECTION("user method: callframe is the Call struct pointer") {
            PyFunc *func = PyFunc::newUserMethod(label, frame, &obj.get());
            void *result = llvmPy_fchk(&callframe, *func, 0);
            CHECK(reinterpret_cast<Call *>(callframe) == &func->getCallFrame());
            CHECK(result != label); // It should point at an unpacking function
        }

    }
}
