#include <llvmPy/PyObj/PyStr.h>
#include <llvmPy/PyObj/PyFunc.h>
#include <llvmPy/PyObj/PyInt.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>
using namespace llvmPy;

static PyFunc *
getMethod(std::string const &name)
{
#define decl(name) \
        {#name, \
         PyFunc::newLibraryFunction( \
            reinterpret_cast<void *>(PyStr::py_##name))}

    static std::unordered_map<std::string, PyFunc *> methods = {
            decl(upper),
            decl(capitalize),
    };
#undef decl

    auto pair = methods.find(name);
    if (pair != methods.end()) {
        return pair->second;
    } else {
        return nullptr;
    }
}

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

size_t
PyStr::getLength() const
{
    return _value->size();
}

std::string const &
PyStr::str() const
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

PyObj *
PyStr::py__getattr__(std::string const &name)
{
    return getMethod(name);
}

PyObj *
PyStr::py_upper(PyStr &self)
{
    if (self.getValue().empty()) {
        return &self;
    }

    auto s = std::make_unique<std::string>(self.getValue());
    std::transform(
            s->begin(),
            s->end(),
            s->begin(),
            [](uint8_t c) { return toupper(c); });
    return new PyStr(std::move(s));
}

PyObj *
PyStr::py_capitalize(PyStr &self)
{
    if (self.getValue().empty()) {
        return &self;
    }

    auto s = std::make_unique<std::string>(self.getValue());
    std::transform(
            s->begin(),
            s->end(),
            s->begin(),
            [](uint8_t c) { return tolower(c); });
    (*s)[0] = static_cast<char>(toupper((*s)[0]));
    return new PyStr(std::move(s));
}

PyObj *
PyStr::py__getitem__(PyObj &key)
{
    auto index = key.as<PyInt>().getValue();
    auto length = static_cast<int64_t>(getLength());

    if (index < 0) {
        index = length + index;
    }

    if (index < 0 || index >= length) {
        throw std::runtime_error("Index out of bounds");
    }

    return new PyStr(getValue().substr(static_cast<size_t>(index), 1));
}
