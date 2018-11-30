#pragma once

#include <llvmPy/Support/iterator_range.h>
#include <memory>
#include <string>
#include <unordered_map>

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
class StructType;
class Value;
}

namespace llvmPy {

class ScopeInfo {
public:
    struct Slot {
        size_t frameIndex;
    };

    struct Loop {
        llvm::BasicBlock *loopCondBB;
        llvm::BasicBlock *loopEndBB;
    };

public:
    ScopeInfo() noexcept;

    void reset();

    bool verify();

public:
    void setParent(ScopeInfo *parent);

    bool hasParent() const;

    ScopeInfo &getParent() const;

    bool hasSlot(std::string const &name) const;

    void declareSlot(std::string const &name);

    size_t getSlotIndex(std::string const &name) const;

    size_t getSlotCount() const;

    Loop const *getLoop() const;

    void setLoop(Loop const *loop);

    void clearLoop();

    llvm::StructType *getInnerFrameType() const;

    void setInnerFrameType(llvm::StructType *st);

    llvm::StructType *getOuterFrameType() const;

    void setOuterFrameType(llvm::StructType *st);

    llvm::Value *getInnerFramePtrPtr() const;

    void setInnerFramePtrPtr(llvm::Value *ptr);

    llvm::Value *getInnerFramePtr() const;

    void setInnerFramePtr(llvm::Value *ptr);

    size_t getNextCondStmtIndex();

    size_t getNextWhileStmtIndex();

    void setCallFramePtr(llvm::Value *ptr);

    llvm::Value *getCallFramePtr() const;

    bool isAlwaysHeap() const;

    void setIsAlwaysHeap(bool value);

private:
    ScopeInfo *_parent;
    std::unordered_map<std::string, Slot> _slots;
    llvm::Value *_innerFramePtr;
    llvm::Value *_innerFramePtrPtr;
    llvm::StructType *_innerFrameType;
    llvm::StructType *_outerFrameType;
    Loop const *_loop;
    size_t _condStmtCount;
    size_t _whileStmtCount;
    llvm::Value *_callframePtr;
    bool _isAlwaysHeap;
};

} // namespace llvmPy
#endif // __cplusplus
