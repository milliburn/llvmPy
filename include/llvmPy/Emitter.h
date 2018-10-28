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
class RTAtom;
class RTScope;
class RTFuncObj;
class RTModule;
class RTModuleObj;

class Emitter {
public:
    explicit Emitter(Compiler &c) noexcept;

    RTModule *createModule(std::string const &name);

    llvm::Value *emit(RTModule &mod, AST const &ast);
    llvm::Value *emit(RTModule &mod, std::vector<Stmt *> const &stmts);

    RTModule *emitModuleNaked2(std::vector<Stmt *> const &stmts);

    RTModuleObj *
    emitModuleNaked(std::vector<Stmt *> const &stmts, std::string &name);

    RTModuleObj *
    emitModuleNaked(std::vector<Stmt *> const &stmts);

    llvm::Value *emitValue(
            AST const &ast,
            llvm::Module &mod,
            RTScope &scope);

    RTModuleObj *
    emitModule(
            std::vector<Stmt *> const &stmts,
            std::string const &name);

    RTFuncObj *
    emitFunction(
            std::string const &name,
            std::vector<std::string const *> args,
            std::vector<Stmt *> body,
            llvm::Module &module,
            RTScope *scope);

private:
    RT &rt;
    llvm::DataLayout const &dl;
    llvm::LLVMContext &ctx;
    llvm::IRBuilder<> ir;
    Types types;

    llvm::Value * emit(
            AST const &ast,
            llvm::Module &module,
            RTScope &outer);

    llvm::AllocaInst * emitAlloca(RTAtom const &);
    llvm::AllocaInst * emitAlloca(RTAtom const &, std::string const &);
};

} // namespace llvmPy
#endif // __cplusplus
