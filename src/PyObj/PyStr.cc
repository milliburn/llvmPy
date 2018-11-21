#include <llvmPy/PyObj/PyStr.h>
using namespace llvmPy;

PyStr::PyStr(std::unique_ptr<std::string const> value) noexcept
: _value(std::move(value))
{
}

PyStr::PyStr(std::string const &value) noexcept
: PyStr(std::unique_ptr<std::string const>(new std::string(value)))
{
}

std::string const &
PyStr::getValue() const
{
    return *_value;
}

std::string
PyStr::py__str__()
{
    return *_value;
}

bool
PyStr::py__bool__()
{
    return !_value->empty();
}

int64_t
PyStr::py__len__()
{
    return static_cast<int64_t>(_value->size());
}

PyObj *
PyStr::py__add__(PyObj &rhs)
{
    auto const &l = getValue();
    auto const &r = rhs.as<PyStr>().getValue();
    return new PyStr(l + r);
}

bool
PyStr::py__eq__(PyObj &rhs)
{
    if (auto *r = rhs.cast<PyStr>()) {
        return getValue() == r->getValue();
    } else {
        return false;
    }
}

int64_t
PyStr::py__int__()
{
    return std::stoi(getValue());
}
