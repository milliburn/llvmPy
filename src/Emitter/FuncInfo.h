#pragma once

#include <llvmPy/Support/iterator_range.h>
#include <memory>
#include <set>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

class Stmt;

class FuncInfo {
public:
    using ArgNamesIter = iterator_range<std::string const *>;

    FuncInfo() noexcept;

    void reset();

    bool verify();

public:
    void setName(std::string const &name);

    std::string const &getName() const;

    void setStmt(Stmt const &stmt);

    Stmt const &getStmt() const;

    void setArgNames(ArgNamesIter argNames);

    iterator_range<std::string const *> getArgNames() const;

private:
    std::string _name;

    Stmt const *_stmt;

    std::set<std::string> _slotNames;

    std::vector<std::string> _argNames;
};

} // namespace llvmPy
#endif // __cplusplus
