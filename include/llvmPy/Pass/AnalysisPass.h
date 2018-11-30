#pragma once

#include "llvmPy/Pass.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class ModuleTree;
class Stmt;

struct AnalysisPassI {
    std::string moduleName;
    std::shared_ptr<Stmt const> stmt;
};

struct AnalysisPassO {
    std::shared_ptr<ModuleTree const> moduleTree;
};

class AnalysisPass : public Pass<AnalysisPassI, AnalysisPassO> {
public:
    AnalysisPassO run(AnalysisPassI const &input) override;
};

} // namespace llvmPy
#endif // __cplusplus
