#include <llvmPy/PyObj/PyNone.h>
using namespace llvmPy;

static PyNone instance;
extern "C" PyNone * const llvmPy_None = &instance;

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
    return *llvmPy_None;
}
