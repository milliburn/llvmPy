#include <llvmPy/PyObj/PyStr.h>
using namespace llvmPy;

std::string
PyStr::py__str__()
{
    return *value;
}
