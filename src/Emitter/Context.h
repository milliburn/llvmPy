#pragma once

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
}

namespace llvmPy {

class Context {
public:
    struct Loop {
        llvm::BasicBlock *loopCondBB;
        llvm::BasicBlock *loopEndBB;
    };

private:
    Loop const *_loop;
};

} // namespace llvmPy
#endif // __cplusplus
