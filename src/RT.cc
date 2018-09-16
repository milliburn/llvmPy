#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvm/Support/Casting.h>
#include <stdexcept>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::runtime_error;

RTAny *
RTAny::access(RTName const &)
{
    return nullptr;
}

void
RTAny::assign(RTName const &, RTAny *)
{
}

RTValue &
RTScope::addSlot(std::string const &name)
{
    auto result = slots.insert(make_pair(name, new RTValue()));

    if (!result.second) {
        throw EmitterError("Slot " + name + " already exists.");
    }

    return *slots[name]; // XXX: Get from the insert.
}
