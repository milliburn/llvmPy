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

bool
PyInt::py__eq__(PyObj &rhs)
{
    if (rhs.isa<PyInt>()) {
        return compare(rhs) == 0;
    } else {
        return false;
    }
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

int
PyInt::compare(PyObj &rhs) const
{
    int64_t lv = getValue();
    int64_t rv = rhs.as<PyInt>().getValue();
    return lv == rv ? 0 : lv < rv ? -1 : +1;
}
