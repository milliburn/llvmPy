#include "llvm/ADT/APInt.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT.h>
#include <llvm/IR/Constants.h>
#include <iostream>
#include <stdlib.h>
using namespace llvmPy;
using llvm::cast;
using std::cerr;
using std::endl;

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
        : ctx(ctx)
{
    i8Ptr = llvm::Type::getInt8PtrTy(ctx);

    PyObj = llvm::StructType::create(ctx, "PyObj");
    Ptr = llvm::PointerType::getUnqual(PyObj);
    FrameN = llvm::StructType::create(ctx, "FrameN");
    FrameNPtr = llvm::PointerType::getUnqual(FrameN);
    FrameNPtrPtr = llvm::PointerType::getUnqual(FrameNPtr);
    Func = llvm::FunctionType::get(Ptr, { FrameNPtrPtr }, false);
    FuncPtr = llvm::PointerType::getUnqual(Func);

    PyIntValue = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    llvmPy_add = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_int = llvm::FunctionType::get(Ptr, { PyIntValue }, false);
    llvmPy_none = llvm::FunctionType::get(Ptr, {}, false);
    llvmPy_func = llvm::FunctionType::get(
            Ptr, { FrameNPtr, i8Ptr }, false);
    llvmPy_fchk = llvm::FunctionType::get(
            i8Ptr, { FrameNPtrPtr, Ptr, PyIntValue }, false);
    llvmPy_print = llvm::FunctionType::get(Ptr, { Ptr }, false);
    llvmPy_str = llvm::FunctionType::get(
            Ptr, { llvm::Type::getInt8PtrTy(ctx) }, false);
    llvmPy_bool = llvm::FunctionType::get(Ptr, { PyIntValue }, false);

    llvm::FunctionType *cmp = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_lt = cmp;
    llvmPy_le = cmp;
    llvmPy_eq = cmp;
    llvmPy_ne = cmp;
    llvmPy_ge = cmp;
    llvmPy_gt = cmp;
}

llvm::StructType *
Types::getFrameN() const
{
    return FrameN;
}

llvm::StructType *
Types::getFrameN(int N) const
{
    if (frameN.count(N)) {
        return frameN[N];
    }

    llvm::StructType *st = llvm::StructType::create(
            ctx, "Frame" + std::to_string(N));
    st->setBody(
            {
                    llvm::PointerType::getUnqual(st),
                    FrameNPtr,
                    llvm::ArrayType::get(Ptr, N)
            },
            true);
    frameN[N] = st;
    return st;
}

llvm::PointerType *
Types::getFrameNPtr() const
{
    return FrameNPtr;
}

llvm::PointerType *
Types::getFrameNPtr(int N) const
{
    return llvm::PointerType::getUnqual(getFrameN(N));
}

llvm::ConstantInt *
Types::getInt32(int32_t value) const
{
    return llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(ctx), (uint64_t) value);
}

llvm::ConstantInt *
Types::getInt64(int64_t value) const
{
    return llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx), (uint64_t) value);
}

llvm::FunctionType *
Types::getFuncN(int N) const
{
    if (funcN.count(N)) {
        return funcN[N];
    }

    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(FrameNPtrPtr);

    for (int i = 0; i < N; ++i) {
        argTypes.push_back(Ptr);
    }

    auto *ft = llvm::FunctionType::get(Ptr, argTypes, false);

    funcN[N] = ft;

    return ft;
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
            break;
        }

    default:
        break;
    }

    cerr << "Cannot add " << l.py__str__()
         << " and " << r.py__str__()
         << "." << endl;
    exit(1);
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
llvmPy_func(FrameN *frame, void *label)
{
    uint64_t prefix = ((uint64_t *) label)[-1];
    auto rtfunc = reinterpret_cast<RTFunc *>(prefix);
    return new PyFunc(rtfunc, frame, label);
}

/**
 * @brief Check a function call against the function's signature.
 * @param np Count of positional arguments passed by the caller.
 * @return Pointer to the function's IR.
 */
extern "C" void *
llvmPy_fchk(FrameN **callframe, llvmPy::PyFunc &pyfunc, int np)
{
    *callframe = &pyfunc.getFrame();
    return pyfunc.getLabel();
}

/**
 * @brief Print the str() of `obj` to stdout. The current implementation is a
 * divergence from Python's print() behaviour.
 * @param obj The object to print.
 */
extern "C" llvmPy::PyObj *
llvmPy_print(llvmPy::PyObj &obj)
{
    std::string str = obj.py__str__();
    std::cout << str << std::endl;
    return PyNone::get();
}

/**
 * @brief Return a PyStr representing the underlying string given.
 */
extern "C" llvmPy::PyStr *
llvmPy_str(uint8_t const *string)
{
    auto copy = std::make_unique<std::string const>((char const *) string);
    return new PyStr(std::move(copy));
}

extern "C" llvmPy::PyBool *
llvmPy_bool(uint64_t value)
{
    return &PyBool::get(value != 0);
}

extern "C" llvmPy::PyBool *
llvmPy_lt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__lt__(r));
}

extern "C" llvmPy::PyBool *
llvmPy_le(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__le__(r));
}

extern "C" llvmPy::PyBool *
llvmPy_eq(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__eq__(r));
}

extern "C" llvmPy::PyBool *
llvmPy_ne(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ne__(r));
}

extern "C" llvmPy::PyBool *
llvmPy_ge(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ge__(r));
}

extern "C" llvmPy::PyBool *
llvmPy_gt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__gt__(r));
}
