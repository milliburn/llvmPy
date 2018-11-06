#pragma once
#include "llvm/Target/TargetMachine.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

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
    llvm::ExecutionEngine &getExecutionEngine() const { return ee; }

private:
    llvm::LLVMContext ctx;
    llvm::TargetMachine &tm;
    llvm::DataLayout const dl;
    llvm::ExecutionEngine &ee;
    RT &rt;
};

} // namespace llvmPy
#endif // __cplusplus
