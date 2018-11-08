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

    llvm::Value *emit(RTScope &scope, AST const &ast);
    llvm::Value *emit(RTScope &scope, IntLitExpr const &expr);
    llvm::Value *emit(RTScope &scope, IdentExpr const &ident);
    llvm::Value *emit(RTScope &scope, CallExpr const &call);
    llvm::Value *emit(RTScope &scope, LambdaExpr const &lambda);
    llvm::Value *emit(RTScope &scope, DefStmt const &def);
    llvm::Value *emit(RTScope &scope, StrLitExpr const &lit);
    llvm::Value *emit(RTScope &scope, BinaryExpr const &expr);

    RTModule *createModule(
            std::string const &name,
            std::vector<Stmt *> const &stmts);

    RTFunc *createFunction(
            std::string const &name,
            RTScope &scope,
            std::vector<Stmt *> const &stmts,
            std::vector<std::string const> const &args);

private:
    llvm::DataLayout const &dl;
    llvm::LLVMContext &ctx;
    llvm::IRBuilder<> ir;
    Types const types;
};

} // namespace llvmPy
#endif // __cplusplus
