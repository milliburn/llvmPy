#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Instr.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "llvm/IR/NoFolder.h"

#ifdef __cplusplus
namespace llvm {
class DataLayout;
class IntegerType;
class Module;
class Value;
} // namespace llvm

namespace llvmPy {

class RT;
class RTScope;
class RTAny;
class RTFunc;

class Emitter {
public:
    explicit Emitter(RT &, llvm::DataLayout &) noexcept;

    llvm::Value * emit(
            AST const &ast,
            llvm::Module &module,
            RTScope &scope);

    llvm::Module * emitModule(
            std::vector<Stmt *> const &stmts,
            std::string const &name);

    llvm::Function * emitFunction(
            AST const &ast,
            llvm::Module &module,
            RTScope &scope);

private:
    RT &rt;
    llvm::DataLayout const &dataLayout;
    llvm::LLVMContext ctx;
    llvm::IRBuilder<> ir;
    Types types;

    llvm::Value * address(RTAny &);
    llvm::Value * address(RTAny *);

    RTFunc * func(AST const &, llvm::Module &, RTScope &);
    llvm::Value * emit(RTAtom const &);
    llvm::Value * ptr(RTAtom const &);
};

} // namespace llvmPy
#endif // __cplusplus
