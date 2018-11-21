#include <llvmPy/PyObj/PyBool.h>
using namespace llvmPy;

PyBool llvmPy_True(true);
PyBool llvmPy_False(false);

PyBool::PyBool(bool value) noexcept
: value_(value)
{

}

bool
PyBool::getValue() const
{
    return value_;
}

std::string
PyBool::py__str__()
{
    return value_ ? "True" : "False";
}

PyBool &
PyBool::get(bool value)
{
    return value ? llvmPy_True : llvmPy_False;
}

bool
PyBool::py__bool__()
{
    return value_;
}
