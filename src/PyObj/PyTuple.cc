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
        ss << _members[i]->py__str__();
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
    return _count > 0;
}

int64_t
PyTuple::py__len__()
{
    return static_cast<int64_t>(_count);
}
