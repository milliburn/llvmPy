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
static RTNone const RTNone_INSTANCE;

RTAny *
RTAny::access(RTName const &)
{
    return nullptr;
}

void
RTAny::assign(RTName const &, RTAny *)
{
}

RTNone const &
RTNone::getInstance()
{
    return RTNone_INSTANCE;
}

RTScope &
RTScope::getNullScope()
{
    return NULL_SCOPE;
}
