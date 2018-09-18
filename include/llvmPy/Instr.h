#pragma once

#ifdef __cplusplus

namespace llvm {
class DataLayout;
class IntegerType;
class LLVMContext;
class FunctionType;
class StructType;
class Type;
} // namespace llvm

namespace llvmPy {

class RTAtom;
class RTAny;

class Types {
public:
    Types(llvm::LLVMContext &, llvm::DataLayout const &);

    llvm::IntegerType *RawPtr;
    llvm::StructType *RTAtom;
    llvm::Type *RTAtomPtr;
    llvm::FunctionType *lpy_add;
};

}

extern "C" {

void lpy_add(llvmPy::RTAtom *, llvmPy::RTAtom &, llvmPy::RTAtom &);
void lpy_eq(llvmPy::RTAtom *, llvmPy::RTAtom &, llvmPy::RTAtom &);

} // extern "C"

#endif // __cplusplus
