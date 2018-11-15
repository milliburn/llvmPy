#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/Instr.h>
#include <llvm/IR/IRBuilder.h>

#ifdef __cplusplus
namespace llvm {
class DataLayout;
class BasicBlock;
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
    /** Details of the loop construct currently in scope. */
    struct Loop {
        llvm::BasicBlock *cond;
        llvm::BasicBlock *end;
    };

    explicit Emitter(Compiler &c) noexcept;

    llvm::Value *emit(RTScope &scope, AST const &ast);
    llvm::Value *emit(RTScope &scope, IntLitExpr const &expr);
    llvm::Value *emit(RTScope &scope, IdentExpr const &ident);
    llvm::Value *emit(RTScope &scope, CallExpr const &call);
    llvm::Value *emit(RTScope &scope, LambdaExpr const &lambda);
    llvm::Value *emit(RTScope &scope, DefStmt const &def);
    llvm::Value *emit(RTScope &scope, StrLitExpr const &lit);
    llvm::Value *emit(RTScope &scope, BinaryExpr const &expr);

    void emitCondStmt(
            llvm::Function &function,
            RTScope &scope,
            ConditionalStmt const &cond,
            Loop const *loop);

    void emitStatement(
            llvm::Function &function,
            RTScope &scope,
            Stmt const &stmt,
            Loop const *loop);

    void emitWhileStmt(
            llvm::Function &function,
            RTScope &scope,
            WhileStmt const &stmt);

    RTModule *createModule(std::string const &name, Stmt const &stmt);
    RTModule *createModule(std::string const &name);

    RTFunc *createFunction(
            std::string const &name,
            RTScope &scope,
            Stmt const &stmt,
            std::vector<std::string const> const &args);

private:
    llvm::DataLayout const &dl;
    llvm::LLVMContext &ctx;
    llvm::IRBuilder<> ir;
    Types const types;
};

} // namespace llvmPy
#endif // __cplusplus
