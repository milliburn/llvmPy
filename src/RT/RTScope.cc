#include <assert.h>
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

bool
RTScope::hasSlot(std::string const &name) const
{
    return slots.count(name) > 0;
}

void
RTScope::declareSlot(std::string const &name)
{
    if (!hasSlot(name)) {
        slots[name] = {
                .value = nullptr,
                .frameIndex = slots.size()
        };
    }
}

llvm::Value *
RTScope::getSlotValue(std::string const &name) const
{
    assert(hasSlot(name));
    return slots.at(name).value;
}

void
RTScope::setSlotValue(std::string const &name, llvm::Value *value)
{
    assert(hasSlot(name));
    slots.at(name).value = value;
}

size_t
RTScope::getSlotIndex(std::string const &name) const
{
    assert(hasSlot(name));
    return slots.at(name).frameIndex;
}
