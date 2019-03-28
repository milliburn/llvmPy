#include <llvmPy/PyObj/PyTuple.h>
#include <cstring>
#include <sstream>
#include <cmath>
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

PyObj * const *
PyTuple::copyMembers(int64_t count, PyObj **members)
{
    assert(count > 0);
    assert(members);
    auto **result = new PyObj* [static_cast<size_t>(count)];
    std::memcpy(
            result,
            members,
            sizeof(*result) * static_cast<size_t>(count));
    return result;
}

PyTuple::PyTuple() noexcept
    : _count(0), _members(nullptr)
{
}

PyTuple::PyTuple(int64_t count, PyObj **members) noexcept
    : _count(count), _members(copyMembers(count, members))
{
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
        if (getLength() != rhs->getLength()) {
            return false;
        } else {
            return compareTo(*rhs, &PyObj::py__eq__);
        }
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
        if (isEmpty() && !rhs->isEmpty()) {
            return true;
        } else if (!isEmpty() && rhs->isEmpty()) {
            return false;
        } else {
            return compareTo(*rhs, &PyObj::py__lt__);
        }
    } else {
        return false;
    }
}

bool
PyTuple::py__le__(PyObj &rhs_)
{
    if (PyObj::py__eq__(rhs_)) {
        return true;
    } else if (auto *rhs = rhs_.cast<PyTuple>()) {
        if (isEmpty() && !rhs->isEmpty()) {
            return true;
        } else if (!isEmpty() && rhs->isEmpty()) {
            return false;
        } else {
            return compareTo(*rhs, &PyObj::py__le__);
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
        if (isEmpty() && !rhs->isEmpty()) {
            return false;
        } else if (!isEmpty() && rhs->isEmpty()) {
            return true;
        } else {
            return compareTo(*rhs, &PyObj::py__gt__);
        }
    } else {
        return false;
    }
}

bool
PyTuple::py__ge__(PyObj &rhs_)
{
    if (PyObj::py__eq__(rhs_)) {
        return true;
    } else if (auto *rhs = rhs_.cast<PyTuple>()) {
        if (isEmpty() && !rhs->isEmpty()) {
            return false;
        } else if (!isEmpty() && rhs->isEmpty()) {
            return true;
        } else {
            return compareTo(*rhs, &PyObj::py__ge__);
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
    assert(_count >= 0);
    return _count;
}

PyObj &
PyTuple::at(int64_t index) const
{
    assert(index < _count);
    auto *member = _members[index];
    assert(member);
    return *member;
}

bool
PyTuple::compareTo(PyTuple &rhs_, bool (PyObj::* comparator)(PyObj &)) const
{
    auto &lhs = *this;
    auto &rhs = rhs_;
    auto len = std::min(lhs.getLength(), rhs.getLength());

    if (len == 0) {
        // It's the empty tuple.
        return true;
    }

    for (int64_t i = 0; i < len; ++i) {
        auto &a = lhs.at(i);
        auto &b = rhs.at(i);
        bool result = (a.*comparator)(b);
        if (result) {
            return true;
        }
    }

    return false;
}
