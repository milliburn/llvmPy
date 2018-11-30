#pragma once

#include <string>

#ifdef __cplusplus
namespace llvmPy {

class FuncTree;

class ModuleTree {
public:

private:
    std::string _name;

    FuncTree const *_funcTree;
};

} // namespace llvmPy
#endif // __cplusplus
