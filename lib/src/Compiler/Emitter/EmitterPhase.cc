#include <assert.h>
#include <llvmPy/Compiler/Emitter/EmitterPhase.h>
using namespace llvmPy;

static std::string const phaseName = "ir";

EmitterPhase::EmitterPhase(Emitter &emitter)
    : Phase(phaseName), _emitter(emitter)
{

}

std::unique_ptr<RTModule>
EmitterPhase::run(Stmt const &stmt)
{
    RTModule *module = _emitter.createModule("__main__", stmt);
    return std::unique_ptr<RTModule>(module);
}
