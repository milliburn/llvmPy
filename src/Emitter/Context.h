#pragma once

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
}

namespace llvmPy {

class FuncInfo;
class ScopeInfo;

class Context {
public:
    Context(FuncInfo const &fi, ScopeInfo const &si);

    void setLoop(llvm::BasicBlock *cond, llvm::BasicBlock *end);

    llvm::BasicBlock *getLoopCondBB() const;

    llvm::BasicBlock *getLoopEndBB() const;

    void resetLoop();

private:
    FuncInfo const &funcInfo;
    ScopeInfo const &scopeInfo;
    llvm::BasicBlock *_loopCondBB;
    llvm::BasicBlock *_loopEndBB;
};

} // namespace llvmPy
#endif // __cplusplus
