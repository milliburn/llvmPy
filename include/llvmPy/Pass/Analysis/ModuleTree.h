#pragma once

#include <string>
#include <llvmPy/Pass/Analysis/LexicalTree.h>

#ifdef __cplusplus
namespace llvmPy {

class FuncTree;
class ScopeTree;

class ModuleTree : public LexicalTree {
public:
    std::string const &getName() const;

    void setName(std::string const &name);

    FuncTree &getFuncTree() const;

    ScopeTree &getScopeTree() const;

    void setBody(std::shared_ptr<LexicalTree> const &tree);

private:

    std::string _name;

    FuncTree *_funcTree;
};

} // namespace llvmPy
#endif // __cplusplus
