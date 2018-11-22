#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

TEST_CASE("instr: llvmPy_fchk()", "[instr][fchk]") {
    SECTION("it will return the LLVM function and frame pointers") {
        auto frame = reinterpret_cast<Frame*>(123);
        auto label = reinterpret_cast<void *>(888);

        PyFunc f(frame, label);
        Frame *callframe;
        void *rv = llvmPy_fchk(&callframe, f, 0);
        CHECK(rv == label);
        CHECK(callframe == frame);
    }
}
