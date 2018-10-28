#include <llvmPy/PyObj.h>
using namespace llvmPy;

static PyNone const PY_NONE;

PyNone const &
PyNone::get()
{
    return PY_NONE;
}
