#include <llvmPy/RT.h>
#include <stdexcept>
using namespace llvmPy;
using std::make_pair;
using std::runtime_error;

void
RT::setMainModule(RTScope *scope)
{
    if (scope->parent) {
        throw runtime_error("The __main__ module cannot have a parent scope.");
    }

    bool ok = modules.insert(make_pair("__main__", scope)).second;

    if (!ok) {
        throw runtime_error("The __main__ module already exists.");
    }
}
