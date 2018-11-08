#include <llvmPy/PyObj/PyInt.h>
#include <sstream>
using namespace llvmPy;

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
