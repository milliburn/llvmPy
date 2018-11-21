#include <assert.h>
#include <llvmPy/RT/RTScope.h>
using namespace llvmPy;

RTScope::RTScope(RTScope &parent)
: Scope(parent),
  _module(parent.getModule()),
  _innerFramePtrPtr(nullptr),
  _innerFrameType(nullptr),
  _outerFrameType(nullptr),
  _condStmtCount(0),
  _whileStmtCount(0)
{
}

RTScope::RTScope(RTModule &module)
: Scope(),
  _module(module),
  _innerFramePtrPtr(nullptr),
  _innerFrameType(nullptr),
  _outerFrameType(nullptr),
  _condStmtCount(0),
  _whileStmtCount(0)
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
    return _module;
}

llvm::Value *
RTScope::getInnerFramePtrPtr() const
{
    return _innerFramePtrPtr;
}

llvm::StructType *
RTScope::getInnerFrameType() const
{
    assert(_innerFrameType);
    return _innerFrameType;
}

void
RTScope::setInnerFrameType(llvm::StructType *st)
{
    _innerFrameType = st;
}

llvm::StructType *
RTScope::getOuterFrameType() const
{
    assert(_outerFrameType);
    return _outerFrameType;
}

void
RTScope::setOuterFrameType(llvm::StructType *st)
{
    _outerFrameType = st;
}

void
RTScope::setInnerFramePtrPtr(llvm::Value *ptr)
{
    _innerFramePtrPtr = ptr;
}

bool
RTScope::hasSlot(std::string const &name) const
{
    return _slots.count(name) > 0;
}

void
RTScope::declareSlot(std::string const &name)
{
    if (!hasSlot(name)) {
        _slots[name] = {
                .value = nullptr,
                .frameIndex = _slots.size()
        };
    }
}

llvm::Value *
RTScope::getSlotValue(std::string const &name) const
{
    assert(hasSlot(name));
    return _slots.at(name).value;
}

void
RTScope::setSlotValue(std::string const &name, llvm::Value *value)
{
    assert(hasSlot(name));
    _slots.at(name).value = value;
}

size_t
RTScope::getSlotIndex(std::string const &name) const
{
    assert(hasSlot(name));
    return _slots.at(name).frameIndex;
}

size_t
RTScope::getSlotCount() const
{
    return _slots.size();
}

llvm::Value *
RTScope::getInnerFramePtr() const
{
    return _innerFramePtr;
}

void
RTScope::setInnerFramePtr(llvm::Value *ptr)
{
    _innerFramePtr = ptr;
}

size_t
RTScope::getNextCondStmtIndex()
{
    return _condStmtCount++;
}

size_t
RTScope::getNextWhileStmtIndex()
{
    return _whileStmtCount++;
}
