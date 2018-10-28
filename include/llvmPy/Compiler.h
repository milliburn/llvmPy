#pragma once
#include "llvm/Target/TargetMachine.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>

#ifdef __cplusplus
namespace llvmPy {

class RT;

class Compiler {
public:
    explicit Compiler(RT &rt) noexcept;

    RT &getRT() const { return rt; }
    llvm::LLVMContext &getContext() { return ctx; }
    llvm::DataLayout const &getDataLayout() const { return dl; }
    llvm::TargetMachine &getTargetMachine() const { return tm; }

private:
    llvm::LLVMContext ctx;
    llvm::TargetMachine &tm;
    llvm::DataLayout const dl;
    RT &rt;
};

} // namespace llvmPy
#endif // __cplusplus
