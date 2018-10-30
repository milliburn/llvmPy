#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/Instr.h>
#include <llvm/IR/IRBuilder.h>

#ifdef __cplusplus
namespace llvm {
class DataLayout;
class IntegerType;
class LLVMContext;
class Module;
class Value;
} // namespace llvm

namespace llvmPy {

class RT;
class RTScope;
class RTModule;

class Emitter {
public:
    explicit Emitter(Compiler &c) noexcept;

    RTModule *createModule(std::string const &name);

    llvm::Value *emit(RTModule &mod, AST const &ast);
    llvm::Value *emit(RTModule &mod, std::vector<Stmt *> const &stmts);
    llvm::Value *emit(RTModule &mod, IntLitExpr const &expr);
    llvm::Value *emit(RTModule &mod, CallExpr const &call);

    RTFunc *emitFunc(
            std::string const &name,
            RTModule &mod,
            std::vector<Stmt *> const &stmts);

private:
    RT &rt;
    llvm::DataLayout const &dl;
    llvm::LLVMContext &ctx;
    llvm::IRBuilder<> ir;
    Types const types;
};

} // namespace llvmPy
#endif // __cplusplus
