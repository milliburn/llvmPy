#pragma once

#include <stdint.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT/Frame.h>
#include <map>

#ifdef __cplusplus

namespace llvm {
class DataLayout;
class IntegerType;
class ConstantInt;
class LLVMContext;
class Function;
class FunctionType;
class PointerType;
class StructType;
class Type;
} // namespace llvm

namespace llvmPy {

class Types {
public:
    Types(llvm::LLVMContext &, llvm::DataLayout const &);

    llvm::StructType *PyObj; ///< Opaque structure type.
    llvm::PointerType *Ptr; ///< Pointer to PyObj.
    llvm::FunctionType *Func; ///< Opaque function.
    llvm::PointerType *FuncPtr; ///< Pointer to opaque function.
    llvm::StructType *FrameN; ///< Frame of unknown size (opaque).
    llvm::PointerType *FrameNPtr;
    llvm::PointerType *FrameNPtrPtr;

    llvm::PointerType *i8Ptr; ///< Equivalent to void*.

    llvm::StructType *getFrameN() const;
    llvm::PointerType *getFrameNPtr() const;
    llvm::StructType *getFrameN(int N) const;
    llvm::PointerType *getFrameNPtr(int N) const;

    llvm::IntegerType *PyIntValue;

    llvm::FunctionType *llvmPy_binop;

    llvm::FunctionType *llvmPy_add;
    llvm::FunctionType *llvmPy_sub;
    llvm::FunctionType *llvmPy_int;
    llvm::FunctionType *llvmPy_none;
    llvm::FunctionType *llvmPy_func;
    llvm::FunctionType *llvmPy_fchk;
    llvm::FunctionType *llvmPy_print;
    llvm::FunctionType *llvmPy_str;
    llvm::FunctionType *llvmPy_bool;
    llvm::FunctionType *llvmPy_truthy;
    llvm::FunctionType *llvmPy_len;

    llvm::FunctionType *llvmPy_lt;
    llvm::FunctionType *llvmPy_le;
    llvm::FunctionType *llvmPy_eq;
    llvm::FunctionType *llvmPy_ne;
    llvm::FunctionType *llvmPy_ge;
    llvm::FunctionType *llvmPy_gt;

    llvm::ConstantInt *getInt32(int32_t value) const;
    llvm::ConstantInt *getInt64(int64_t value) const;

    llvm::FunctionType *getFuncN(int N) const;

private:
    llvm::LLVMContext &ctx;
    std::map<int, llvm::StructType *> mutable frameN;
    std::map<int, llvm::FunctionType *> mutable funcN;
};

}

extern "C" {

extern llvmPy::PyNone llvmPy_None;
extern llvmPy::PyBool llvmPy_True;
extern llvmPy::PyBool llvmPy_False;

llvmPy::PyObj *llvmPy_add(llvmPy::PyObj &, llvmPy::PyObj &);
llvmPy::PyObj *llvmPy_sub(llvmPy::PyObj &, llvmPy::PyObj &);
llvmPy::PyObj *llvmPy_mul(llvmPy::PyObj &, llvmPy::PyObj &);
llvmPy::PyInt *llvmPy_int(llvmPy::PyObj &);
llvmPy::PyNone *llvmPy_none();
llvmPy::PyFunc *llvmPy_func(llvmPy::Frame *frame, void *label);
void *llvmPy_fchk(llvmPy::Frame **callframe, llvmPy::PyFunc &pyfunc, int np);
llvmPy::PyObj *llvmPy_print(llvmPy::PyObj &);
llvmPy::PyStr *llvmPy_str(llvmPy::PyObj &);
llvmPy::PyBool *llvmPy_bool(llvmPy::PyObj &);

llvmPy::PyBool *llvmPy_lt(llvmPy::PyObj &l, llvmPy::PyObj &r);
llvmPy::PyBool *llvmPy_le(llvmPy::PyObj &l, llvmPy::PyObj &r);
llvmPy::PyBool *llvmPy_eq(llvmPy::PyObj &l, llvmPy::PyObj &r);
llvmPy::PyBool *llvmPy_ne(llvmPy::PyObj &l, llvmPy::PyObj &r);
llvmPy::PyBool *llvmPy_ge(llvmPy::PyObj &l, llvmPy::PyObj &r);
llvmPy::PyBool *llvmPy_gt(llvmPy::PyObj &l, llvmPy::PyObj &r);

llvmPy::PyInt *llvmPy_len(llvmPy::PyObj &);

uint8_t llvmPy_truthy(llvmPy::PyObj &);

} // extern "C"

#endif // __cplusplus
