#pragma once

#include <stdint.h>
#include <llvmPy/PyObj.h>

#ifdef __cplusplus

namespace llvm {
class DataLayout;
class IntegerType;
class LLVMContext;
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

    llvm::IntegerType *PyIntValue;

    llvm::FunctionType *llvmPy_add;
    llvm::FunctionType *llvmPy_int;
    llvm::FunctionType *llvmPy_none;
    llvm::FunctionType *llvmPy_func;

    static constexpr unsigned long CALL_N_COUNT = 2;
    llvm::FunctionType *llvmPy_callN[CALL_N_COUNT];
};

}

extern "C" {

llvmPy::PyObj *llvmPy_add(llvmPy::PyObj &, llvmPy::PyObj &);
llvmPy::PyInt *llvmPy_int(int64_t value);
llvmPy::PyNone *llvmPy_none();
llvmPy::PyObj *llvmPy_call0(llvmPy::PyFunc &func);
llvmPy::PyObj *llvmPy_call1(llvmPy::PyFunc &func, llvmPy::PyObj &arg0);
llvmPy::PyFunc *llvmPy_func(llvm::Function *function);

} // extern "C"

#endif // __cplusplus
