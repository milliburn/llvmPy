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

#define __used __attribute__((used))

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
        : ctx(ctx)
{
    i8Ptr = llvm::Type::getInt8PtrTy(ctx);

    PyObj = llvm::StructType::create(ctx, "PyObj");
    Ptr = llvm::PointerType::getUnqual(PyObj);
    Frame = llvm::StructType::create(ctx, "Frame");
    FramePtr = getPtr(Frame);
    FramePtrPtr = getPtr(FramePtr);

    PyIntValue = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    llvmPy_binop = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_add = llvmPy_binop;
    llvmPy_sub = llvmPy_binop;
    llvmPy_int = llvm::FunctionType::get(Ptr, { Ptr }, false);
    llvmPy_none = llvm::FunctionType::get(Ptr, {}, false);
    llvmPy_func = llvm::FunctionType::get(Ptr, { FramePtr, i8Ptr }, false);
    llvmPy_fchk = llvm::FunctionType::get(
            i8Ptr,
            { FramePtrPtr,
              Ptr,
              PyIntValue },
            false);
    llvmPy_print = llvm::FunctionType::get(Ptr, { Ptr }, false);
    llvmPy_str = llvm::FunctionType::get(Ptr, { Ptr }, false);
    llvmPy_bool = llvm::FunctionType::get(Ptr, { Ptr }, false);
    llvmPy_truthy = llvm::FunctionType::get(
            llvm::Type::getInt1Ty(ctx), { Ptr }, false);

    llvmPy_len = llvm::FunctionType::get(Ptr, { Ptr }, false);

    llvm::FunctionType *cmp = llvmPy_binop;
    llvmPy_lt = cmp;
    llvmPy_le = cmp;
    llvmPy_eq = cmp;
    llvmPy_ne = cmp;
    llvmPy_ge = cmp;
    llvmPy_gt = cmp;
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

llvm::PointerType *
Types::getPtr(llvm::Type *type) const
{
    return llvm::PointerType::getUnqual(type);
}

llvm::FunctionType *
Types::getOpaqueFunc(int N) const
{
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(FramePtrPtr);

    for (int i = 0; i < N; ++i) {
        argTypes.push_back(Ptr);
    }

    return llvm::FunctionType::get(Ptr, argTypes, false);
}

llvm::FunctionType *
Types::getFunc(
        std::string const &name,
        llvm::StructType *outer,
        int N) const
{
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(getPtr(getPtr(outer)));

    for (int i = 0; i < N; ++i) {
        argTypes.push_back(Ptr);
    }

    return llvm::FunctionType::get(Ptr, argTypes, false);
}

llvm::StructType *
Types::getFuncFrame(
        std::string const &name,
        llvm::StructType *outer,
        int N) const
{
    std::string frameName = "Frame." + name;

    auto *structType = llvm::StructType::create(ctx, frameName);

    structType->setBody(
            { getPtr(structType),
              getPtr(outer),
              llvm::ArrayType::get(Ptr, N), },
            true);

    return structType;
}

extern "C" PyObj * __used
llvmPy_add(PyObj &l, PyObj &r)
{
    return l.py__add__(r);
}

extern "C" PyObj * __used
llvmPy_sub(PyObj &l, PyObj &r)
{
    return l.py__sub__(r);
}

extern "C" llvmPy::PyObj * __used
llvmPy_mul(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return l.py__mul__(r);
}

extern "C" PyInt * __used
llvmPy_int(llvmPy::PyObj &obj)
{
    return new PyInt(obj.py__int__());
}

extern "C" llvmPy::PyNone * __used
llvmPy_none()
{
    return &PyNone::get();
}

static Frame *
moveFrameToHeap(Frame *stackFrame, Scope const &scope)
{
    if (!stackFrame || !stackFrame->self) {
        // The frame is already on the heap.
        return stackFrame;
    } else {
        // The frame is currently on the stack.

        auto slotCount = scope.getSlotCount();

        auto frameSize = (
                sizeof(Frame) +
                slotCount * sizeof(stackFrame->vars[0]));

        auto *heapFrame = reinterpret_cast<Frame *>(malloc(frameSize));
        memcpy(heapFrame, stackFrame, frameSize);

        heapFrame->self = nullptr; // Marks that it's on the heap.
        stackFrame->self = heapFrame;
        heapFrame->outer = moveFrameToHeap(
                stackFrame->outer, scope.getParent());
        return heapFrame;
    }
}

extern "C" PyFunc * __used
llvmPy_func(Frame *stackFrame, void *label)
{
    // The `label` is that of the callee, but llvmPy_func() itself operates
    // within the caller; hence the scope->getParent().

    auto *scope = reinterpret_cast<Scope const **>(label)[-1];
    auto &parent = scope->getParent();
    Frame *heapFrame = moveFrameToHeap(stackFrame, parent);
    auto *pyfunc = new PyFunc(heapFrame, label);
    return pyfunc;
}

/**
 * @brief Check a function call against the function's signature.
 * @param np Count of positional arguments passed by the caller.
 * @return Pointer to the function's IR.
 */
extern "C" void * __used
llvmPy_fchk(Frame **callframe, llvmPy::PyFunc &pyfunc, int np)
{
    *callframe = pyfunc.getFrame();
    return pyfunc.getLabel();
}

/**
 * @brief Print the str() of `obj` to stdout.
 * @param obj The object to print.
 */
extern "C" llvmPy::PyObj * __used
llvmPy_print(llvmPy::PyObj &obj)
{
    std::string str = obj.py__str__();
    std::cout << str << std::endl;
    return &PyNone::get();
}

/**
 * @brief Return a PyStr representing the underlying string given.
 */
extern "C" llvmPy::PyStr * __used
llvmPy_str(llvmPy::PyObj &obj)
{
    return new PyStr(obj.py__str__());
}

/**
 * TODO: Stop using methods for generating constants.
 */
extern "C" llvmPy::PyBool * __used
llvmPy_bool(llvmPy::PyObj &obj)
{
    return &PyBool::get(obj.py__bool__());
}

extern "C" llvmPy::PyBool * __used
llvmPy_lt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__lt__(r));
}

extern "C" llvmPy::PyBool * __used
llvmPy_le(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__le__(r));
}

extern "C" llvmPy::PyBool * __used
llvmPy_eq(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__eq__(r));
}

extern "C" llvmPy::PyBool * __used
llvmPy_ne(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ne__(r));
}

extern "C" llvmPy::PyBool * __used
llvmPy_ge(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ge__(r));
}

extern "C" llvmPy::PyBool * __used
llvmPy_gt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__gt__(r));
}

/**
 * Return the truthiness of `obj`. This operation is much like llvmPy_bool(),
 * but the result is immediately converted into a i1 value of 1 or 0 for
 * branching instructions.
 */
extern "C" uint8_t __used
llvmPy_truthy(llvmPy::PyObj &obj)
{
    return static_cast<uint8_t>(obj.py__bool__() ? 1 : 0);
}

extern "C" llvmPy::PyInt *
llvmPy_len(llvmPy::PyObj &obj)
{
    auto len = obj.py__len__();
    return new PyInt(len);
}
