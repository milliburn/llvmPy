#include <llvmPy/PyObj/PyInt.h>
#include <llvm/Support/Casting.h>
#include <sstream>
using namespace llvmPy;
using llvm::cast;

PyInt::PyInt(int64_t value)
: PyObj(PyObjType::Int), value(value)
{
}

int64_t
PyInt::getValue() const
{
    return value;
}

std::string
PyInt::py__str__()
{
    return std::to_string(value);
}

bool
PyInt::py__lt__(PyObj &rhs)
{
    if (rhs.getType() == PyObjType::Int) {
        int64_t rv = cast<PyInt>(rhs).getValue();
        return getValue() < rv;
    } else {
        return false;
    }
}

bool
PyInt::py__le__(PyObj &rhs)
{
    if (rhs.getType() == PyObjType::Int) {
        int64_t rv = cast<PyInt>(rhs).getValue();
        return getValue() <= rv;
    } else {
        return false;
    }
}

bool
PyInt::py__eq__(PyObj &rhs)
{
    if (rhs.getType() == PyObjType::Int) {
        int64_t rv = cast<PyInt>(rhs).getValue();
        return getValue() == rv;
    } else {
        return false;
    }
}

bool
PyInt::py__ge__(PyObj &rhs)
{
    if (rhs.getType() == PyObjType::Int) {
        int64_t rv = cast<PyInt>(rhs).getValue();
        return getValue() >= rv;
    } else {
        return false;
    }
}

bool
PyInt::py__gt__(PyObj &rhs)
{
    if (rhs.getType() == PyObjType::Int) {
        int64_t rv = cast<PyInt>(rhs).getValue();
        return getValue() > rv;
    } else {
        return false;
    }
}
