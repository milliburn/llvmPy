#include <llvmPy/PyObj/PyBool.h>
using namespace llvmPy;

static PyBool TRUE(true), FALSE(false);

PyBool::PyBool(bool value) noexcept
: PyObj(PyObjType::Bool), value(value)
{

}

bool
PyBool::getValue() const
{
    return value;
}

std::string
PyBool::py__str__()
{
    return value ? "True" : "False";
}

PyBool &
PyBool::get(bool value)
{
    return value ? TRUE : FALSE;
}
