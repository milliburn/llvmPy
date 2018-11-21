#include <llvmPy/PyObj/PyInt.h>
#include <sstream>
using namespace llvmPy;

PyInt::PyInt(int64_t value)
: _value(value)
{
}

int64_t
PyInt::getValue() const
{
    return _value;
}

std::string
PyInt::py__str__()
{
    return std::to_string(_value);
}

PyObj *
PyInt::py__add__(PyObj &rhs)
{
    int64_t lv = getValue();
    int64_t rv = rhs.as<PyInt>().getValue();
    return new PyInt(lv + rv);
}

PyObj *
PyInt::py__sub__(PyObj &rhs)
{
    int64_t lv = getValue();
    int64_t rv = rhs.as<PyInt>().getValue();
    return new PyInt(lv - rv);
}

PyObj *
PyInt::py__mul__(PyObj &rhs)
{
    int64_t lv = getValue();
    int64_t rv = rhs.as<PyInt>().getValue();
    return new PyInt(lv * rv);
}

int
PyInt::cmp(PyObj &rhs, bool throwIfInvalid)
{
    int64_t lv = getValue();
    int64_t rv;

    if (auto *pyInt = rhs.cast<PyInt>()) {
        rv = pyInt->getValue();
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

bool
PyInt::py__bool__()
{
    return _value != 0;
}

int64_t
PyInt::py__int__()
{
    return _value;
}
