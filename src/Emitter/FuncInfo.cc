#include "FuncInfo.h"
using namespace llvmPy;

static std::string const EmptyName = "";

FuncInfo::FuncInfo() noexcept
{
    reset();
}

void
FuncInfo::reset()
{
    _name = EmptyName;
    _stmt = nullptr;
    _slotNames.clear();
    _argNames.clear();
}

bool
FuncInfo::verify()
{
    return false;
}

void
FuncInfo::setName(std::string const &name)
{
    _name = name;
}

std::string const &
FuncInfo::getName() const
{
    return _name;
}

void
FuncInfo::setStmt(Stmt const &stmt)
{
    _stmt = &stmt;
}

Stmt const &
FuncInfo::getStmt() const
{
    return *_stmt;
}

void
FuncInfo::setArgNames(FuncInfo::ArgNamesIter argNames)
{
    _argNames.clear();
    for (auto const &argName : argNames) {
        _argNames.emplace_back(argName);
    }
}

iterator_range<std::string const *>
FuncInfo::getArgNames() const
{
    return make_range<std::string const *>(
            _argNames.data(),
            _argNames.data() + _argNames.size());
}
