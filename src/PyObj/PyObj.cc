#include <llvmPy/PyObj.h>
#include <sstream>
#include <llvmPy/PyObj/PyObj.h>

using namespace llvmPy;

PyObj::PyObj() = default;

PyObj::PyObj(PyObj const &copy) noexcept = default;

PyObj::PyObj(PyObj &&move) noexcept = default;

PyObj::~PyObj() = default;

std::string
PyObj::py__str__()
{
    std::stringstream ss;
    ss << "<PyObj 0x";
    ss << std::hex << std::uppercase << reinterpret_cast<uint64_t>(this);
    ss << ">";
    return ss.str();
}

bool
PyObj::py__bool__()
{
    return true;
}

PyObj *
PyObj::py__add__(PyObj &rhs)
{
    return &PyNone::get();
}

PyObj *
PyObj::py__sub__(PyObj &rhs)
{
    return &PyNone::get();
}

PyObj *
PyObj::py__mul__(PyObj &rhs)
{
    return &PyNone::get();
}

bool
PyObj::py__lt__(PyObj &rhs)
{
    return compare(rhs) < 0;
}

bool
PyObj::py__le__(PyObj &rhs)
{
    return compare(rhs) <= 0;
}

bool
PyObj::py__eq__(PyObj &rhs)
{
    return this == &rhs;
}

bool
PyObj::py__ne__(PyObj &rhs)
{
    return !py__eq__(rhs);
}

bool
PyObj::py__ge__(PyObj &rhs)
{
    return compare(rhs) >= 0;
}

bool
PyObj::py__gt__(PyObj &rhs)
{
    return compare(rhs) > 0;
}

int64_t
PyObj::py__len__()
{
    return 0;
}

int64_t
PyObj::py__int__()
{
    return 0;
}

PyObj *
PyObj::py__getattr__(std::string const &name)
{
    return &PyNone::get();
}

bool
PyObj::isInstance() const
{
    return true;
}

int
PyObj::compare(PyObj &rhs) const
{
    throw std::runtime_error("Comparison not supported.");
}
