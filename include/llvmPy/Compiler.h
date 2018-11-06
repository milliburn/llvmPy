#pragma once
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <vector>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class CompilerImpl;

class Compiler {
public:
    explicit Compiler() noexcept;

    void run(
            llvm::Function *function,
            std::vector<llvm::Value *> const &args);

    void *addModule(llvm::Module *module);

public:
    llvm::LLVMContext &getContext() { return ctx; }
    llvm::DataLayout const &getDataLayout() const;
    llvm::TargetMachine &getTargetMachine() const;

private:
    llvm::LLVMContext ctx;
    std::unique_ptr<CompilerImpl> impl;
};

} // namespace llvmPy
#endif // __cplusplus
