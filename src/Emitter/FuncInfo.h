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

private:
    std::string _name;
    std::shared_ptr<Stmt const> _stmt;
    std::set<std::string> _slotNames;
    std::vector<std::string> _argNames;
};

} // namespace llvmPy
#endif // __cplusplus
