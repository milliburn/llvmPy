#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#pragma GCC diagnostic pop

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
    llvm::LLVMContext &getContext() { return _ctx; }
    llvm::DataLayout const &getDataLayout() const;

private:
    llvm::LLVMContext _ctx;
    std::unique_ptr<CompilerImpl> const _impl;
};

} // namespace llvmPy
#endif // __cplusplus
