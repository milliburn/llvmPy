#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/ADT/APInt.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#pragma GCC diagnostic pop

#include <llvmPy/Instr.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT.h>
#include <iostream>
#include <stdlib.h>
using namespace llvmPy;
using llvm::cast;
using std::cerr;
using std::endl;

#define MARK_USED __attribute__((used))

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
        : _ctx(ctx)
{
    i8Ptr = llvm::Type::getInt8PtrTy(ctx);

    PyObj = llvm::StructType::create(ctx, "PyObj");
    Ptr = llvm::PointerType::getUnqual(PyObj);
    Frame = llvm::StructType::create(ctx, "Frame");
    FramePtr = getPtr(Frame);
    FramePtrPtr = getPtr(FramePtr);

    PyIntValue = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    llvmPy_malloc = llvm::FunctionType::get(
            llvm::Type::getInt64Ty(ctx),
            { llvm::Type::getInt64Ty(ctx) },
            false);

    llvmPy_calloc = llvm::FunctionType::get(
            llvm::Type::getInt64Ty(ctx),
            { llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx) },
            false);

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
    llvmPy_getattr = llvm::FunctionType::get(Ptr, { Ptr, Ptr }, false);
    llvmPy_tupleN = llvm::FunctionType::get(
            Ptr,
            { llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx) },
            false);

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
            llvm::Type::getInt32Ty(_ctx), static_cast<uint64_t>(value));
}

llvm::ConstantInt *
Types::getInt64(int64_t value) const
{
    return llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(_ctx), static_cast<uint64_t>(value));
}

llvm::PointerType *
Types::getPtr(llvm::Type *type) const
{
    return llvm::PointerType::getUnqual(type);
}

llvm::FunctionType *
Types::getOpaqueFunc(size_t N) const
{
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(FramePtr);

    for (size_t i = 0; i < N; ++i) {
        argTypes.push_back(Ptr);
    }

    return llvm::FunctionType::get(Ptr, argTypes, false);
}

llvm::FunctionType *
Types::getFunc(
        std::string const &name,
        llvm::StructType *outer,
        size_t N) const
{
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(getPtr(outer));

    for (size_t i = 0; i < N; ++i) {
        argTypes.push_back(Ptr);
    }

    return llvm::FunctionType::get(Ptr, argTypes, false);
}

llvm::StructType *
Types::getFuncFrame(
        std::string const &name,
        llvm::StructType *outer,
        size_t N) const
{
    std::string frameName = "Frame." + name;

    auto *structType = llvm::StructType::create(_ctx, frameName);

    structType->setBody(
            { getPtr(structType),
              getPtr(outer),
              llvm::ArrayType::get(Ptr, N), },
            true);

    return structType;
}

llvm::ArrayType *
Types::getPyObjArray(size_t N) const
{
    return llvm::ArrayType::get(Ptr, N);
}

extern "C" void * MARK_USED
llvmPy_malloc(size_t size)
{
    return malloc(size);
}

extern "C" void * MARK_USED
llvmPy_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

extern "C" PyObj * MARK_USED
llvmPy_add(PyObj &l, PyObj &r)
{
    return l.py__add__(r);
}

extern "C" PyObj * MARK_USED
llvmPy_sub(PyObj &l, PyObj &r)
{
    return l.py__sub__(r);
}

extern "C" llvmPy::PyObj * MARK_USED
llvmPy_mul(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return l.py__mul__(r);
}

extern "C" PyInt * MARK_USED
llvmPy_int(llvmPy::PyObj &obj)
{
    return new PyInt(obj.py__int__());
}

extern "C" llvmPy::PyNone * MARK_USED
llvmPy_none()
{
    return &PyNone::get();
}

Frame *
llvmPy::moveFrameToHeap(Frame *frame, Scope const &scope)
{
    if (!frame) {
        return nullptr;
    } else if (frame->isPointingToHeap()) {
        return frame->self;
    } else if (frame->isOnHeap()) {
        return frame;
    } else {
        auto const slotCount = scope.getSlotCount();
        auto const frameSize = Frame::getSizeof(slotCount);
        auto *heapFrame = reinterpret_cast<Frame *>(malloc(frameSize));
        frame->moveToHeapFrame(heapFrame, slotCount);

        if (scope.hasParent() && heapFrame->outer) {
            // TODO: XXX
            // TODO: Right now the createFunction() that emits the module's
            // TODO: __body__ needs an upper scope, but that doesn't have a
            // TODO: corresponding frame. Hence we can't fully require that
            // TODO: the outer pointer exist at this stage.
            assert(heapFrame->outer);
            heapFrame->outer = moveFrameToHeap(
                    heapFrame->outer,
                    scope.getParent());
        } else {
            heapFrame->outer = nullptr;
        }

        return heapFrame;
    }
}

extern "C" PyFunc * MARK_USED
llvmPy_func(Frame *stackFrame, void *label)
{
    // The `label` is that of the callee, but llvmPy_func() itself operates
    // within the caller; hence the scope->getParent().

    auto *scope = reinterpret_cast<Scope const **>(label)[-1];
    auto &parent = scope->getParent();
    Frame *heapFrame = moveFrameToHeap(stackFrame, parent);
    auto *pyfunc = PyFunc::newUserFunction(label, heapFrame);
    return pyfunc;
}

/**
 * @brief Check a function call against the function's signature.
 * @param np Count of positional arguments passed by the caller.
 * @return Pointer to the function's IR.
 */
extern "C" void * MARK_USED
llvmPy_fchk(Frame **callframe, llvmPy::PyFunc &func, int np)
{
    switch (func.getType()) {
    case PyFuncType::LibraryFunction:
        *callframe = nullptr;
        break;

    case PyFuncType::LibraryMethod:
        *callframe = reinterpret_cast<Frame *>(func.getSelf());
        break;

    case PyFuncType::UserFunction:
        *callframe = func.getFrame();
        break;

    case PyFuncType::UserMethod:
        *callframe = reinterpret_cast<Frame *>(
                const_cast<Call *>(
                        &func.getCallFrame()));
        // TODO: Return the label for the unpacking function.
        return nullptr; // TODO: XXX
    }

    return func.getLabel();
}

/**
 * @brief Print the str() of `obj` to stdout.
 * @param obj The object to print.
 */
extern "C" llvmPy::PyObj * MARK_USED
llvmPy_print(llvmPy::PyObj &obj)
{
    std::string str = obj.py__str__();
    std::cout << str << std::endl;
    return &PyNone::get();
}

/**
 * @brief Return a PyStr representing the underlying string given.
 */
extern "C" llvmPy::PyStr * MARK_USED
llvmPy_str(llvmPy::PyObj &obj)
{
    return new PyStr(obj.py__str__());
}

/**
 * TODO: Stop using methods for generating constants.
 */
extern "C" llvmPy::PyBool * MARK_USED
llvmPy_bool(llvmPy::PyObj &obj)
{
    return &PyBool::get(obj.py__bool__());
}

extern "C" llvmPy::PyTuple * MARK_USED
llvmPy_tupleN(int64_t count, llvmPy::PyObj **objects)
{
    return &PyTuple::getNoCopy(count, objects);
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_lt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__lt__(r));
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_le(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__le__(r));
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_eq(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__eq__(r));
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_ne(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ne__(r));
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_ge(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__ge__(r));
}

extern "C" llvmPy::PyBool * MARK_USED
llvmPy_gt(llvmPy::PyObj &l, llvmPy::PyObj &r)
{
    return &PyBool::get(l.py__gt__(r));
}

/**
 * Return the truthiness of `obj`. This operation is much like llvmPy_bool(),
 * but the result is immediately converted into a i1 value of 1 or 0 for
 * branching instructions.
 */
extern "C" uint8_t MARK_USED
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

extern "C" llvmPy::PyObj *
llvmPy_getattr(llvmPy::PyObj &object, llvmPy::PyStr &name)
{
    PyObj *attr = object.py__getattr__(name.str());

    if (auto *func = attr->cast<PyFunc>()) {
        if (object.isInstance()) {
            assert(!func->isMethod());
            assert(!func->isBound());
            return func->bind(object);
        } else {
            return attr;
        }
    } else {
        return attr;
    }
}
