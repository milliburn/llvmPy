#pragma once

#include <stdint.h>
#include <llvmPy/PyObj.h>
#include <llvmPy/RT/Frame.h>

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

    llvm::StructType *getFrameN() const;
    llvm::PointerType *getFrameNPtr() const;
    llvm::StructType *getFrameN(int N) const;
    llvm::PointerType *getFrameNPtr(int N) const;

    llvm::IntegerType *PyIntValue;

    llvm::FunctionType *llvmPy_add;
    llvm::FunctionType *llvmPy_int;
    llvm::FunctionType *llvmPy_none;
    llvm::FunctionType *llvmPy_func;
    llvm::FunctionType *llvmPy_fchk;

    llvm::ConstantInt *getInt32(int32_t value) const;
    llvm::ConstantInt *getInt64(int64_t value) const;

private:
    llvm::LLVMContext &ctx;
};

}

extern "C" {

llvmPy::PyObj *llvmPy_add(llvmPy::PyObj &, llvmPy::PyObj &);
llvmPy::PyInt *llvmPy_int(int64_t value);
llvmPy::PyNone *llvmPy_none();
llvmPy::PyFunc *llvmPy_func(llvmPy::FrameN *frame, llvm::Function *function);
llvm::Function *llvmPy_fchk(llvmPy::FrameN **callframe, llvmPy::PyFunc &pyfunc, int np);

} // extern "C"

#endif // __cplusplus
