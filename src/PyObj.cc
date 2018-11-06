#include <llvmPy/PyObj.h>
#include <sstream>
using namespace llvmPy;

static PyNone PY_NONE;

std::string
PyObj::py__str__()
{
    std::stringstream ss;
    ss << "<PyObj 0x";
    ss << std::hex << std::uppercase << (uint64_t) this;
    ss << ">";
    return ss.str();
}

PyNone *
PyNone::get()
{
    return &PY_NONE;
}

std::string
PyInt::py__str__()
{
    return std::to_string(value);
}
