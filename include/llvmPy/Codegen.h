#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#ifdef __cplusplus
namespace llvmPy {

class Codegen {
public:
    Codegen();
    llvm::LLVMContext& getContext();
    llvm::IRBuilder<>& getBuilder();

private:
    llvm::LLVMContext ctx;
    llvm::IRBuilder<> ir;
};

} // namespace llvmPy
#endif // __cplusplus

