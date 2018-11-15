#include <llvmPy/PyObj/PyStr.h>
using namespace llvmPy;

PyStr::PyStr(std::unique_ptr<std::string const> value) noexcept
: PyObj(PyObjType::Str), value(std::move(value))
{
}

PyStr::PyStr(std::string const &value) noexcept
: PyStr(std::unique_ptr<std::string const>(new std::string(value)))
{
}

std::string const &
PyStr::getValue() const
{
    return *value;
}

std::string
PyStr::py__str__()
{
    return *value;
}

bool
PyStr::py__bool__()
{
    return !value->empty();
}
