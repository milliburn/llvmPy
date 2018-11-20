#include <assert.h>
#include <llvmPy/RT/RTScope.h>
using namespace llvmPy;

RTScope::RTScope(RTScope &parent)
: Scope(parent),
  module(parent.getModule()),
  innerFramePtrPtr(nullptr),
  innerFrameType(nullptr),
  outerFrameType(nullptr)
{
}

RTScope::RTScope(RTModule &module)
: Scope(),
  module(module),
  innerFramePtrPtr(nullptr),
  innerFrameType(nullptr),
  outerFrameType(nullptr)
{
}

RTScope *
RTScope::createDerived()
{
    return new RTScope(*this);
}

RTModule &
RTScope::getModule() const
{
    return module;
}

llvm::Value *
RTScope::getInnerFramePtrPtr() const
{
    return innerFramePtrPtr;
}

llvm::StructType *
RTScope::getInnerFrameType() const
{
    assert(innerFrameType);
    return innerFrameType;
}

void
RTScope::setInnerFrameType(llvm::StructType *st)
{
    innerFrameType = st;
}

llvm::StructType *
RTScope::getOuterFrameType() const
{
    assert(outerFrameType);
    return outerFrameType;
}

void
RTScope::setOuterFrameType(llvm::StructType *st)
{
    outerFrameType = st;
}

void
RTScope::setInnerFramePtrPtr(llvm::Value *ptr)
{
    innerFramePtrPtr = ptr;
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

llvm::Value *
RTScope::getInnerFramePtr() const
{
    return innerFramePtr;
}

void
RTScope::setInnerFramePtr(llvm::Value *ptr)
{
    innerFramePtr = ptr;
}
