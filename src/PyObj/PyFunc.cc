#include <llvmPy/PyObj/PyFunc.h>
#include <sstream>
using namespace llvmPy;

PyFunc::PyFunc(Frame *frame, void *label)
: frame_(frame), label_(label)
{
}

Frame *
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
    ss << std::hex << std::uppercase << reinterpret_cast<uint64_t>(this);
    ss << ">";
    return ss.str();
}
