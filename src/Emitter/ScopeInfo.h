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
    class Slot {
    };

    struct Loop {
        llvm::BasicBlock *loopCondBB;
        llvm::BasicBlock *loopEndBB;
    };

private:
    std::unordered_map<std::string, Slot> _slots;
    llvm::Value *_innerFramePtr;
    llvm::Value *_innerFramePtrPtr;
    llvm::StructType *_innerFrameType;
    llvm::StructType *_outerFrameType;
    Loop const *_loop;
    size_t _condStmtCount;
    size_t _whileStmtCount;
    llvm::Value *_callframePtr;
};

} // namespace llvmPy
#endif // __cplusplus
