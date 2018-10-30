#include "llvm/ADT/APInt.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT.h>
#include <llvm/IR/Constants.h>

using namespace llvmPy;
using llvm::cast;

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
        : ctx(ctx)
{
    PyObj = llvm::StructType::create(ctx, "PyObj");
    Ptr = llvm::PointerType::getUnqual(PyObj);
    Func = llvm::FunctionType::get(Ptr, {}, false);
    FuncPtr = llvm::PointerType::getUnqual(Func);
    FrameN = llvm::StructType::create(ctx, "FrameN");
    FrameNPtr = llvm::PointerType::getUnqual(FrameN);
    FrameNPtrPtr = llvm::PointerType::getUnqual(FrameNPtr);

    PyIntValue = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    llvmPy_add = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_int = llvm::FunctionType::get(Ptr, { PyIntValue }, false);
    llvmPy_none = llvm::FunctionType::get(Ptr, {}, false);
    llvmPy_func = llvm::FunctionType::get(
            Ptr, { FrameNPtr, PyIntValue }, false);
    llvmPy_fchk = llvm::FunctionType::get(
            FuncPtr, { FrameNPtrPtr, Ptr, PyIntValue }, false);
}

llvm::StructType *
Types::getFrameN() const
{
    return FrameN;
}

llvm::StructType *
Types::getFrameN(int N) const
{
    llvm::StructType *st = llvm::StructType::create(
            ctx, "Frame" + std::to_string(N));
    st->setBody(
            llvm::PointerType::getUnqual(st),
            FrameNPtr,
            llvm::ArrayType::get(Ptr, N));
    return st;
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

extern "C" PyFunc *
llvmPy_func(FrameN *frame, llvm::Function *function)
{
    llvm::Constant *prefix = function->getPrefixData();
    llvm::ConstantInt *value = cast<llvm::ConstantInt>(prefix);
    uint64_t raw = *value->getValue().getRawData();
    return reinterpret_cast<PyFunc *>(raw);
}

/**
 * @brief Check a function call against the function's signature.
 * @param np Count of positional arguments passed by the caller.
 * @return Pointer to the function's IR.
 */
extern "C" llvm::Function *
llvmPy_fchk(FrameN **frame, llvmPy::PyFunc &func, int np)
{
    return &func.getFunc().getFunction();
}
