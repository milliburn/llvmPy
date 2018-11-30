#pragma once

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
class Module;
class Value;
}

namespace llvmPy {

class FuncInfo;
class ScopeInfo;

class Context {
public:
    Context(llvm::Module *mod, FuncInfo const &fi, ScopeInfo const &si);

    void setLoop(llvm::BasicBlock *cond, llvm::BasicBlock *end);

    llvm::BasicBlock *getLoopCondBB() const;

    llvm::BasicBlock *getLoopEndBB() const;

    void resetLoop();

    llvm::Module *getModule() const;

    FuncInfo const &getFuncInfo() const;

    ScopeInfo const &getScopeInfo() const;

    llvm::Value *getFramePtr() const;

    void setFramePtr(llvm::Value *ptr);

    llvm::Value *getFramePtrPtr() const;

    void setFramePtrPtr(llvm::Value *ptrptr);

    llvm::Value *getCallFramePtr() const;

    void setCallFramePtr(llvm::Value *ptr);

private:
    llvm::Module * const _module;
    FuncInfo const &_funcInfo;
    ScopeInfo const &_scopeInfo;
    llvm::BasicBlock *_loopCondBB;
    llvm::BasicBlock *_loopEndBB;
    llvm::Value *_framePtr;
    llvm::Value *_framePtrPtr;
    llvm::Value *_callFramePtr;
};

} // namespace llvmPy
#endif // __cplusplus
