#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <stdexcept>
using namespace llvmPy;
using std::make_pair;
using std::runtime_error;

RTValue &
RTScope::addSlot(std::string const &name)
{
    auto result = slots.insert(make_pair(name, new RTValue()));

    if (!result.second) {
        throw EmitterError("Slot " + name + " already exists.");
    }

    return *slots[name]; // XXX: Get from the insert.
}

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

RTScope &
RT::createScope()
{
    return *new RTScope(*this);
}

RTScope &
RT::createScope(RTScope &parent)
{
    RTScope &scope = createScope();
    scope.parent = &parent;
    return scope;
}
