#include <llvmPy/PyObj/PyTuple.h>
#include <cstring>
#include <sstream>
using namespace llvmPy;

PyTuple llvmPy_tuple0;

PyTuple &
PyTuple::get(int64_t count, PyObj **members)
{
    if (count == 0) {
        return llvmPy_tuple0;
    } else {
        return *new PyTuple(count, members);
    }
}

PyTuple::PyTuple() noexcept
    : _count(0), _members(nullptr)
{

}

PyTuple::PyTuple(int64_t count, PyObj **members) noexcept
    : _count(count), _members(new PyObj* [static_cast<size_t>(count)])
{
    assert(count > 0);
    assert(members);

    std::memcpy(
            _members,
            members,
            sizeof(*_members) * static_cast<size_t>(_count));

    for (int64_t i = 0; i < _count; ++i) {
        assert(_members[i]);
    }
}

std::string
PyTuple::py__str__()
{
    if (_count == 0) {
        return "()";
    }

    std::stringstream ss;
    ss << "(";

    for (int64_t i = 0; i < _count; ++i) {
        if (i > 0) {
            ss << ", ";
        }

        // TODO: Should there be a toStream for this purpose?
        ss << at(i).py__str__();
    }

    if (_count == 1) {
        ss << ",";
    }

    ss << ")";
    return ss.str();
}

bool
PyTuple::py__bool__()
{
    return !isEmpty();
}

int64_t
PyTuple::py__len__()
{
    return getLength();
}

bool
PyTuple::py__eq__(PyObj &rhs_)
{
    if (PyObj::py__eq__(rhs_)) {
        return true;
    } else if (auto *rhs = rhs_.cast<PyTuple>()) {
        return compareTo(*rhs, &PyObj::py__eq__);
    } else {
        return false;
    }
}

bool
PyTuple::py__lt__(PyObj &rhs_)
{
    if (PyObj::py__eq__(rhs_)) {
        return false;
    } else if (auto *rhs = rhs_.cast<PyTuple>()) {
        if (getLength() < rhs->getLength()) {
            return true;
        } else {
            return compareTo(*rhs, &PyObj::py__lt__);
        }
    } else {
        return false;
    }
}

bool
PyTuple::py__gt__(PyObj &rhs_)
{
    if (PyObj::py__eq__(rhs_)) {
        return false;
    } else if (auto *rhs = rhs_.cast<PyTuple>()) {
        if (getLength() > rhs->getLength()) {
            return true;
        } else {
            return compareTo(*rhs, &PyObj::py__gt__);
        }
    } else {
        return false;
    }
}

bool
PyTuple::isEmpty() const
{
    return _count == 0;
}

int64_t
PyTuple::getLength() const
{
    return _count;
}

PyObj &
PyTuple::at(int64_t index) const
{
    assert(index < _count);
    return *_members[index];
}

bool
PyTuple::compareTo(PyTuple &rhs_, bool (PyObj::* comparator)(PyObj &)) const
{
    auto &lhs = *this;
    auto &rhs = rhs_;

    if (lhs.getLength() != rhs.getLength()) {
        return false;
    } else {
        for (int64_t i = 0; i < getLength(); ++i) {
            auto &a = lhs.at(i);
            auto &b = rhs.at(i);
            bool result = (a.*comparator)(b);
            if (!result) {
                return false;
            }
        }

        return true;
    }
}
