#pragma once

#include <memory>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class FuncInfo;
class ScopeInfo;
class Stmt;

class ModuleInfo {
public:
    ModuleInfo();

private:
    std::string _name;

    std::unique_ptr<FuncInfo> _bodyFuncInfo;

    std::unique_ptr<ScopeInfo> _bodyScopeInfo;
};

} // namespace llvmPy
#endif // __cplusplus
