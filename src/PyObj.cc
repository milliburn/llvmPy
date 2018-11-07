#include <llvmPy/PyObj.h>
#include <llvmPy/RT.h>
#include <llvm/IR/Function.h>
#include <sstream>
using namespace llvmPy;

static PyNone PY_NONE;

std::string
PyObj::py__str__()
{
    std::stringstream ss;
    ss << "<PyObj 0x";
    ss << std::hex << std::uppercase << (uint64_t) this;
    ss << ">";
    return ss.str();
}

PyNone *
PyNone::get()
{
    return &PY_NONE;
}

std::string
PyInt::py__str__()
{
    return std::to_string(value);
}

std::string
PyNone::py__str__()
{
    return "None";
}

std::string
PyFunc::py__str__()
{
    std::stringstream ss;
    ss << "<PyFunc ";
    ss << func.getFunction().getName().str();
    ss << " at ";
    ss << std::hex << std::uppercase << (uint64_t) this;
    ss << ">";
    return ss.str();
}
