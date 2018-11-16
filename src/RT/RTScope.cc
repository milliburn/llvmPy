#include <llvmPy/RT.h>
using namespace llvmPy;

RTScope::RTScope(
        RTModule &module,
        RTScope &parent,
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
        : module(module),
          parent(&parent),
          outerFramePtr(outerFramePtr),
          innerFramePtr(innerFramePtr)
{

}

RTScope::RTScope(RTModule &module)
        : module(module),
          parent(nullptr),
          outerFramePtr(nullptr),
          innerFramePtr(nullptr)
{
}

RTScope *
RTScope::createDerived(
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
{
    return new RTScope(module, *this, innerFramePtr, outerFramePtr);
}

RTScope &
RTScope::getParent() const
{
    if (!hasParent()) {
        throw "Cannot return parent of top-level RTScope.";
    }

    return *parent;
}

RTModule &
RTScope::getModule() const
{
    return module;
}

bool
RTScope::hasParent() const
{
    return parent != nullptr;
}

llvm::Value *
RTScope::getOuterFramePtr() const
{
    return outerFramePtr;
}

llvm::Value *
RTScope::getInnerFramePtr() const
{
    return innerFramePtr;
}
