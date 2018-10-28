#include <llvmPy/PyObj.h>
using namespace llvmPy;

static PyNone PY_NONE;

PyNone *
PyNone::get()
{
    return &PY_NONE;
}
