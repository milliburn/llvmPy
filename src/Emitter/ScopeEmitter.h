#pragma once

#include <unordered_map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/IRBuilder.h>
#pragma GCC diagnostic pop

#include <llvmPy/AST.h>
#include <llvmPy/Support/iterator_range.h>

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
class DataLayout;
class LLVMContext;
class Value;
} // namespace llvm

namespace llvmPy {

class Compiler;
class ModuleEmitter;

class ScopeEmitter {
public:
    struct Loop {
        llvm::BasicBlock *cond;
        llvm::BasicBlock *end;
    };

    struct Slot {
        llvm::Value *value;
        size_t frameIndex;
    };

    ScopeEmitter(
            Compiler &compiler,
            ModuleEmitter const &module) noexcept;

    virtual ~ScopeEmitter();

private:
    llvm::LLVMContext &_ctx;
    llvm::DataLayout const &_dl;

protected:
    ModuleEmitter const &_module;
    llvm::IRBuilder<> _ir;

private:
    std::unordered_map<std::string, Slot> _slots;
};

} // namespace llvmPy
#endif // __cplusplus
