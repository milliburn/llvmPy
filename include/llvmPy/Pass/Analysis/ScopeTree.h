#pragma once

#include <string>

#ifdef __cplusplus
namespace llvmPy {

class ScopeTree {
public:

    ScopeTree &getParent() const;

    bool hasParent() const;

    void setParent(ScopeTree &parent);

    bool hasSlot(std::string const &name) const;

    void declareSlot(std::string const &name);

    size_t getSlotIndex(std::string const &name) const;

    size_t getSlotCount() const;

    size_t getNextCondStmtIndex();

    size_t getNextWhileStmtIndex();

private:
};

} // namespace llvmPy
#endif // __cplusplus
