#pragma once

#include <memory>
#include <string>
#include <llvmPy/Support/iterator_range.h>

#ifdef __cplusplus
namespace llvmPy {

class ScopeTree;
class Stmt;

class FuncTree {
public:

    using ArgNamesIter = iterator_range<std::string const *>;

    void setName(std::string const &name);

    std::string const &getName() const;

    void setStmt(Stmt const &stmt);

    Stmt const &getStmt() const;

    void setArgNames(ArgNamesIter argNames);

    iterator_range<std::string const *> getArgNames() const;

    size_t getArgCount() const;

private:

    std::string _name;

    ScopeTree *_scopeTree;

    std::shared_ptr<Stmt const> _stmt;
};

} // namespace llvmPy
#endif // __cplusplus
