#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/IRBuilder.h>
#pragma GCC diagnostic pop

#include <llvmPy/AST.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Support/iterator_range.h>
#include <set>

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

class FuncInfo;
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

    llvm::Value *emit(RTScope &scope, Expr const &expr);
    llvm::Value *emit(RTScope &scope, IntegerExpr const &expr);
    llvm::Value *emit(RTScope &scope, IdentExpr const &ident);
    llvm::Value *emit(RTScope &scope, CallExpr const &call);
    llvm::Value *emit(RTScope &scope, LambdaExpr const &lambda);
    llvm::Value *emit(RTScope &scope, StringExpr const &lit);
    llvm::Value *emit(RTScope &scope, BinaryExpr const &expr);
    llvm::Value *emit(RTScope &scope, UnaryExpr const &unary);
    llvm::Value *emit(RTScope &scope, GetattrExpr const &getattr);

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

    void emitDefStmt(
            llvm::Function &function,
            RTScope &scope,
            DefStmt const &def);

    void emitBreakStmt(Loop const *loop);
    void emitContinueStmt(Loop const *loop);

    void gatherSlotNames(
            Stmt const &stmt,
            std::set<std::string> &names);

    llvm::Value *findLexicalSlotGEP(
            std::string const &name,
            RTScope &scope,
            llvm::Value *framePtrPtr = nullptr);

    RTModule *createModule(std::string const &name, Stmt const &stmt);
    RTModule *createModule(std::string const &name);

    llvm::Function *createFunction(RTScope &scope, FuncInfo const &funcInfo);

private:
    llvm::DataLayout const &_dl;
    llvm::LLVMContext &_ctx;
    llvm::IRBuilder<> _ir;
    Types const _types;

    bool lastInstructionWasTerminator() const;
};

} // namespace llvmPy
#endif // __cplusplus
