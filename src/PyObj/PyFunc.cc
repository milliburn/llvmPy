#include <llvmPy/PyObj/PyFunc.h>
#include <sstream>
using namespace llvmPy;

PyFunc::PyFunc(Frame *frame, void *label)
: _frame(frame), _label(label)
{
}

Frame *
PyFunc::getFrame() const
{
    return _frame;
}

void *
PyFunc::getLabel() const
{
    return _label;
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
