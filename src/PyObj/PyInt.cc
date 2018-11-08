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

PyObj &
PyInt::py__add__(PyObj &rhs)
{
    int64_t lv = getValue();
    int64_t rv;

    if (rhs.getType() == PyObjType::Int) {
        rv = cast<PyInt>(rhs).getValue();
    } else {
        throw "Invalid right-hand operator";
    }

    return *new PyInt(lv + rv);
}

PyObj &
PyInt::py__mul__(PyObj &rhs)
{
    int64_t lv = getValue();
    int64_t rv;

    if (rhs.getType() == PyObjType::Int) {
        rv = cast<PyInt>(rhs).getValue();
    } else {
        throw "Invalid right-hand operator";
    }

    return *new PyInt(lv * rv);
}

int
PyInt::cmp(PyObj &rhs, bool throwIfInvalid)
{
    int64_t lv = getValue();
    int64_t rv;

    if (rhs.getType() == PyObjType::Int) {
        rv = cast<PyInt>(rhs).getValue();
    } else {
        if (throwIfInvalid) {
            throw "Invalid comparison!";
        } else {
            return InvalidCmp;
        }
    }

    return lv == rv ? 0 : lv < rv ? -1 : +1;
}

bool
PyInt::py__lt__(PyObj &rhs)
{
    return cmp(rhs, true) < 0;
}

bool
PyInt::py__le__(PyObj &rhs)
{
    return cmp(rhs, true) <= 0;
}

bool
PyInt::py__eq__(PyObj &rhs)
{
    int sgn = cmp(rhs, false);
    return sgn == InvalidCmp ? false : sgn == 0;
}

bool
PyInt::py__ge__(PyObj &rhs)
{
    return cmp(rhs, true) >= 0;
}

bool
PyInt::py__gt__(PyObj &rhs)
{
    return cmp(rhs, true) > 0;
}
