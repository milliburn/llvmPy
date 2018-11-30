#include "ScopeInfo.h"
using namespace llvmPy;

ScopeInfo::ScopeInfo() noexcept
{
    reset();
}

void
ScopeInfo::reset()
{
    _slots.clear();
    _innerFramePtr = nullptr;
    _innerFramePtrPtr = nullptr;
    _innerFrameType = nullptr;
    _outerFrameType = nullptr;
    _loop = nullptr;
    _condStmtCount = 0;
    _whileStmtCount = 0;
    _callframePtr = nullptr;
}

bool
ScopeInfo::verify()
{
    return true;
}

bool
ScopeInfo::hasSlot(std::string const &name) const
{
    return _slots.count(name) > 0;
}

void
ScopeInfo::declareSlot(std::string const &name)
{
    if (!hasSlot(name)) {
        _slots[name] = {
                .value = nullptr,
                .frameIndex = _slots.size()
        };
    }
}

size_t
ScopeInfo::getSlotIndex(std::string const &name) const
{
    assert(hasSlot(name));
    return _slots.at(name).frameIndex;
}

size_t
ScopeInfo::getSlotCount() const
{
    return _slots.size();
}

ScopeInfo::Loop const *
ScopeInfo::getLoop() const
{
    return _loop;
}

void
ScopeInfo::setLoop(ScopeInfo::Loop const *loop)
{
    _loop = loop;
}

void
ScopeInfo::clearLoop()
{
    _loop = nullptr;
}

llvm::StructType *
ScopeInfo::getInnerFrameType() const
{
    return _innerFrameType;
}

void
ScopeInfo::setInnerFrameType(llvm::StructType *st)
{
    _innerFrameType = st;
}

llvm::StructType *
ScopeInfo::getOuterFrameType() const
{
    return _outerFrameType;
}

void
ScopeInfo::setOuterFrameType(llvm::StructType *st)
{
    _outerFrameType = st;
}

llvm::Value *
ScopeInfo::getInnerFramePtrPtr() const
{
    return _innerFramePtrPtr;
}

void
ScopeInfo::setInnerFramePtrPtr(llvm::Value *ptr)
{
    _innerFramePtrPtr = ptr;
}

llvm::Value *
ScopeInfo::getInnerFramePtr() const
{
    return _innerFramePtr;
}

void
ScopeInfo::setInnerFramePtr(llvm::Value *ptr)
{
    _innerFramePtr = ptr;
}

size_t
ScopeInfo::getNextCondStmtIndex()
{
    return _condStmtCount++;
}

size_t
ScopeInfo::getNextWhileStmtIndex()
{
    return _whileStmtCount++;
}

void
ScopeInfo::setCallFramePtr(llvm::Value *ptr)
{
    _callframePtr = ptr;
}

llvm::Value *
ScopeInfo::getCallFramePtr() const
{
    return _callframePtr;
}

bool
ScopeInfo::isAlwaysHeap() const
{
    return _isAlwaysHeap;
}

void
ScopeInfo::setIsAlwaysHeap(bool value)
{
    _isAlwaysHeap = value;
}
