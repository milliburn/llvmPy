#include <assert.h>
#include <llvmPy/RT/RTScope.h>
using namespace llvmPy;

RTScope::RTScope(
        RTModule &module,
        RTScope &parent,
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
        : Scope(parent),
          module(module),
          outerFramePtr(outerFramePtr),
          innerFramePtr(innerFramePtr)
{

}

RTScope::RTScope(RTModule &module)
        : Scope(),
          module(module),
          outerFramePtr(nullptr),
          innerFramePtr(nullptr)
{
}

RTScope *
RTScope::createDerived(
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
{
    // TODO: Assert not null.
    return new RTScope(module, *this, innerFramePtr, outerFramePtr);
}

RTModule &
RTScope::getModule() const
{
    return module;
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

size_t
RTScope::getSlotCount() const
{
    return slots.size();
}
