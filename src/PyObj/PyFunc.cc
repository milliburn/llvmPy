#include <llvmPy/PyObj/PyFunc.h>
#include <sstream>
using namespace llvmPy;

PyFunc::PyFunc(FrameN *frame, void *label)
: frame_(frame), label_(label)
{
}

FrameN *
PyFunc::getFrame() const
{
    return frame_;
}

void *
PyFunc::getLabel() const
{
    return label_;
}

std::string
PyFunc::py__str__()
{
    std::stringstream ss;
    ss << "<PyFunc ";
    ss << std::hex << std::uppercase << (uint64_t) this;
    ss << ">";
    return ss.str();
}
