#include <llvmPy/Pass/AnalysisPass.h>
#include <llvmPy/Pass/Analysis/FuncTree.h>
#include <llvmPy/Pass/Analysis/ModuleTree.h>
#include <llvmPy/Pass/Analysis/ScopeTree.h>
using namespace llvmPy;

AnalysisPassO
AnalysisPass::run(AnalysisPassI const &input)
{
    auto moduleTree = std::make_shared<ModuleTree>();
    moduleTree->setName(input.moduleName);



    return AnalysisPassO({ .moduleTree = std::move(moduleTree) });
}

void
AnalysisPass::initScopeSlotsFromSignature(
        ScopeTree &st,
        FuncTree const &ft) const
{
    for (auto const &argName : ft.getArgNames()) {
        st.declareSlot(argName);
    }
}

void
AnalysisPass::initScopeSlotsFromBody(
        ScopeTree &st,
        Stmt const &stmt) const
{
    if (auto *assign = stmt.cast<AssignStmt>()) {
        st.declareSlot(assign->getName());
    } else if (auto *def = stmt.cast<DefStmt>()) {
        st.declareSlot(def->getName());
    } else if (auto *compound = stmt.cast<CompoundStmt>()) {
        for (auto const &stmt_ : compound->getStatements()) {
            initScopeSlotsFromBody(st, *stmt_);
        }
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        initScopeSlotsFromBody(st, cond->getThenBranch());
        initScopeSlotsFromBody(st, cond->getElseBranch());
    } else if (auto *loop = stmt.cast<WhileStmt>()) {
        initScopeSlotsFromBody(st, loop->getBody());
    }
}

void
AnalysisPass::initScopeFrames(
        ScopeTree &st,
        FuncTree const &ft) const
{

}
