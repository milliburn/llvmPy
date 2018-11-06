#pragma once
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
    ~Compiler();
    void addAndRunModule(std::unique_ptr<llvm::Module> module);

public:
    llvm::LLVMContext &getContext() { return ctx; }
    llvm::DataLayout const &getDataLayout() const;

private:
    llvm::LLVMContext ctx;
    std::unique_ptr<CompilerImpl> const impl;
};

} // namespace llvmPy
#endif // __cplusplus
