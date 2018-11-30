#pragma once

#include <memory>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class ScopeTree;
class Stmt;

class FuncTree {
public:

private:
    std::string _name;

    ScopeTree const *_scopeTree;

    std::shared_ptr<Stmt const> _stmt;
};

} // namespace llvmPy
#endif // __cplusplus
