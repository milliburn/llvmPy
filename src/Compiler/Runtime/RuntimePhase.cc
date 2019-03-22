#include <assert.h>
#include <llvmPy/Compiler/Runtime/RuntimePhase.h>
using namespace llvmPy;

static std::string const phaseName = "rt";

RuntimePhase::RuntimePhase(RT &rt)
    : Phase(phaseName), _rt(rt)
{

}

void
RuntimePhase::run(RTModule &module)
{
    _rt.import(module);
}
