#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvm/Support/Casting.h>
#include <stdexcept>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::runtime_error;

static RTScope NULL_SCOPE;

RTAny *
RTAny::access(RTName const &)
{
    return nullptr;
}

void
RTAny::assign(RTName const &, RTAny *)
{
}

RTScope &
RTScope::getNullScope()
{
    return NULL_SCOPE;
}
