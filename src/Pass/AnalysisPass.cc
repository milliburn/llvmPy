#include <llvmPy/Pass/AnalysisPass.h>
using namespace llvmPy;

AnalysisPassO
AnalysisPass::run(AnalysisPassI const &input)
{
    return AnalysisPassO();
}

void
AnalysisPass::initScopeSlotsFromSignature(ScopeTree &, FuncTree const &) const
{

}

void
AnalysisPass::initScopeSlotsFromBody(ScopeTree &, Stmt const &) const
{

}

void
AnalysisPass::initScopeFrames(ScopeTree &, FuncTree const &) const
{

}
