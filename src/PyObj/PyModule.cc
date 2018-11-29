#include <llvmPy/PyObj/PyModule.h>
#include <sstream>
using namespace llvmPy;

static std::string const empty("");

std::string
PyModule::py__str__()
{
    std::stringstream ss;
    ss << "<PyModule " << getName();

    if (!getPath().empty()) {
        ss << " at " << getPath();
    }

    ss << ">";
    return ss.str();
}

std::string const &
PyModule::getName() const
{
    return empty;
}

std::string const &
PyModule::getPath() const
{
    return empty;
}

PyObj *
PyModule::py__getattr__(std::string const &name)
{
    return nullptr;
}

void
PyModule::py__setattr__(std::string const &name, PyObj &obj)
{
}
