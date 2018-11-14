#include <llvmPy/PyObj.h>
#include <sstream>
#include <llvmPy/PyObj/PyObj.h>

using namespace llvmPy;

std::string
PyObj::py__str__()
{
    std::stringstream ss;
    ss << "<PyObj 0x";
    ss << std::hex << std::uppercase << (uint64_t) this;
    ss << ">";
    return ss.str();
}

bool
PyObj::py__bool__()
{
    return true;
}

PyObj &
PyObj::py__add__(PyObj &rhs)
{
    return PyNone::get();
}

PyObj &
PyObj::py__mul__(PyObj &rhs)
{
    return PyNone::get();
}

bool
PyObj::py__lt__(PyObj &)
{
    return false;
}

bool
PyObj::py__le__(PyObj &)
{
    return false;
}

bool
PyObj::py__eq__(PyObj &)
{
    return false;
}

bool
PyObj::py__ne__(PyObj &rhs)
{
    return !py__eq__(rhs);
}

bool
PyObj::py__ge__(PyObj &)
{
    return false;
}

bool
PyObj::py__gt__(PyObj &)
{
    return false;
}
