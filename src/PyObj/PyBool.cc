#include <llvmPy/PyObj/PyBool.h>
using namespace llvmPy;

static PyBool trueInstance(true);
static PyBool falseInstance(false);

extern "C" PyBool * const llvmPy_True = &trueInstance;
extern "C" PyBool * const llvmPy_False = &falseInstance;

PyBool::PyBool(bool value) noexcept
: PyObj(PyObjType::Bool), value(value)
{

}

bool
PyBool::getValue() const
{
    return value;
}

std::string
PyBool::py__str__()
{
    return value ? "True" : "False";
}

PyBool &
PyBool::get(bool value)
{
    return value ? *llvmPy_True : *llvmPy_False;
}
