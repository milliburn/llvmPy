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

    RawPtr = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    RTType = RawPtr;

    RTAtom = llvm::StructType::create(
            ctx,
            { RTType, RawPtr },
            "RTAtom",
            true);

    RTAtomPtr = llvm::PointerType::getUnqual(RTAtom);

    lpy_add = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    lpy_int = llvm::FunctionType::get(Ptr, { PyIntValue }, false);
}

extern "C" PyObj *
lpy_add(PyObj &l, PyObj &r)
{
    switch (l.getType()) {
    case Type::Int:
        switch (r.getType()) {
        case Type::Int: {
            PyInt &lhs = cast<PyInt>(l);
            PyInt &rhs = cast<PyInt>(r);
            return new PyInt(lhs.getValue() + rhs.getValue());
        }

        case Type::Dec: {
            PyInt &lhs = cast<PyInt>(l);
            PyDec &rhs = cast<PyDec>(r);
            return new PyDec(lhs.getValue() + rhs.getValue());
        }

        default:
            return new PyNone();

        }

    case Type::Dec: {
        switch (r.getType()) {
        case Type::Int: {
            PyDec &lhs = cast<PyDec>(l);
            PyInt &rhs = cast<PyInt>(r);
            return new PyDec(lhs.getValue() + rhs.getValue());
        }

        case Type::Dec: {
            PyDec &lhs = cast<PyDec>(l);
            PyDec &rhs = cast<PyDec>(r);
            return new PyDec(lhs.getValue() + rhs.getValue());
        }

        default:
            return new PyNone();

        }
    }

    default:
        return new PyNone();
    }
}

extern "C" PyInt *
lpy_int(int64_t value)
{
    return new PyInt(value);
}
