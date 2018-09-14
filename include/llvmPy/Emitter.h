#pragma once
#include <llvmPy/AST.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "llvm/IR/NoFolder.h"

#ifdef __cplusplus
namespace llvm {
class Module;
class Value;
} // namespace llvm

namespace llvmPy {

class RT;
class RTScope;

class Emitter {
public:
    explicit Emitter(RT &) noexcept;
    llvm::Value * emit(
            AST const &ast,
            llvm::Module &module,
            RTScope &scope);
    llvm::Module * emitModule(
            std::vector<Stmt *> const &stmts,
            std::string const &name);

private:
    RT &rt;
    llvm::LLVMContext ctx;
    // TODO: Reintroduce constant folding.
    llvm::IRBuilder<llvm::NoFolder> ir;
};

} // namespace llvmPy
#endif // __cplusplus
