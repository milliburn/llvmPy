#include <llvmPy/PyObj/PyFunc.h>
#include <sstream>
using namespace llvmPy;

std::ostream &
llvmPy::operator<<(std::ostream &s, PyFuncType const &t)
{
    switch (t) {
    case PyFuncType::LibraryFunction: s << "LF"; break;
    case PyFuncType::LibraryMethod: s << "LM"; break;
    case PyFuncType::UserFunction: s << "UF"; break;
    case PyFuncType::UserMethod: s << "UM"; break;
    }

    return s;
}

PyFunc *
PyFunc::newLibraryFunction(void *label)
{
    return new PyFunc(PyFuncType::LibraryFunction, label, nullptr, nullptr);
}

PyFunc *
PyFunc::newLibraryMethod(void *label, PyObj *obj)
{
    return new PyFunc(PyFuncType::LibraryMethod, label, nullptr, obj);
}

PyFunc *
PyFunc::newUserFunction(void *label, Frame *frame)
{
    return new PyFunc(PyFuncType::UserFunction, label, frame, nullptr);
}

PyFunc *
PyFunc::newUserMethod(void *label, Frame *frame, PyObj *obj)
{
    return new PyFunc(PyFuncType::UserMethod, label, frame, obj);
}

PyFunc::PyFunc(PyFunc const &copy) noexcept = default;

PyFunc::PyFunc(PyFunc &&move) noexcept = default;

PyFunc::PyFunc(PyFuncType type, void *label, Frame *frame, PyObj *self)
: _type(type),
  _call(frame, label, self)
{
}

PyFuncType
PyFunc::getType() const
{
    return _type;
}

Frame *
PyFunc::getFrame() const
{
    return _call.frame;
}

void *
PyFunc::getLabel() const
{
    return _call.label;
}

Call const &
PyFunc::getCallFrame() const
{
    return _call;
}

PyObj *
PyFunc::getSelf() const
{
    return _call.self;
}

std::string
PyFunc::py__str__()
{
    std::stringstream ss;
    ss << "<PyFunc ";
    ss << getType();
    ss << " 0x";
    ss << std::hex << std::uppercase << reinterpret_cast<uint64_t>(this);
    ss << ">";
    return ss.str();
}

bool
PyFunc::isBound() const
{
    return _call.self != nullptr;
}

bool
PyFunc::isMethod() const
{
    return (getType() == PyFuncType::LibraryMethod ||
            getType() == PyFuncType::UserMethod);
}

PyFunc *
PyFunc::bind(PyObj &self) const
{
    assert(!isBound());

    if (getType() == PyFuncType::LibraryFunction) {
        return newLibraryMethod(getLabel(), &self);
    } else if (getType() == PyFuncType::UserFunction) {
        return newUserMethod(getLabel(), getFrame(), &self);
    } else {
        assert(false);
        return nullptr;
    }
}
