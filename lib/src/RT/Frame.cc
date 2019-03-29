#include <assert.h>
#include <string.h>
#include <llvmPy/RT/Frame.h>
using namespace llvmPy;

static Frame * const FrameIsHeap = reinterpret_cast<Frame *>(~0);

Frame::Frame() noexcept
: self(nullptr),
  outer(nullptr)
{

}

bool
Frame::isPointingToHeap() const
{
    return outer == FrameIsHeap;
}

bool
Frame::isOnHeap() const
{
    return self == nullptr;
}

void
Frame::setIsPointingToHeap()
{
    outer = FrameIsHeap;
}

void
Frame::moveToHeapFrame(Frame *heapFrame, size_t N)
{
    assert(!isPointingToHeap());
    auto const size = getSizeof(N);
    memcpy(heapFrame, this, size);
    heapFrame->self = nullptr;
    memset(this, 0, size);
    this->self = heapFrame;
    this->outer = FrameIsHeap;
}
