#include <llvmPy/PyObj/PyNone.h>
using namespace llvmPy;

PyNone llvmPy_None;

PyNone::PyNone() noexcept
: PyObj(PyObjType::None)
{
}

std::string
PyNone::py__str__()
{
    return "None";
}

PyNone &
PyNone::get()
{
    return llvmPy_None;
}

bool
PyNone::py__bool__()
{
    return false;
}
