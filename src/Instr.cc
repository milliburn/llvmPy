#include "llvm/ADT/APInt.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT.h>
using namespace llvmPy;
using llvm::cast;

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
{
    PyObj = llvm::StructType::create(ctx, "PyObj");
    Ptr = llvm::PointerType::getUnqual(PyObj);

    PyIntValue = llvm::IntegerType::get(ctx, 64);

    llvmPy_add = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_int = llvm::FunctionType::get(Ptr, { PyIntValue }, false);
    llvmPy_none = llvm::FunctionType::get(Ptr, {}, false);

    for (int i = 0; i < CALL_N_COUNT; ++i) {
        std::vector<llvm::Type *> args(i, Ptr);
        llvmPy_callN[i] = llvm::FunctionType::get(Ptr, args, false);
    }
}

extern "C" PyObj *
llvmPy_add(PyObj &l, PyObj &r)
{
    switch (l.getType()) {
    case PyObjType::Int:
        switch (r.getType()) {
        case PyObjType::Int: {
            PyInt &lhs = cast<PyInt>(l);
            PyInt &rhs = cast<PyInt>(r);
            return new PyInt(lhs.getValue() + rhs.getValue());
        }

        default:
            return new PyNone();

        }

    default:
        return new PyNone();
    }
}

extern "C" PyInt *
llvmPy_int(int64_t value)
{
    return new PyInt(value);
}

extern "C" llvmPy::PyNone *
llvmPy_none()
{
    return PyNone::get();
}

extern "C" llvmPy::PyObj *
llvmPy_call0(llvmPy::PyFunc &func)
{
    return nullptr;
}

extern "C" llvmPy::PyObj *
llvmPy_call1(llvmPy::PyFunc &func, llvmPy::PyObj &arg0)
{
    return nullptr;
}
