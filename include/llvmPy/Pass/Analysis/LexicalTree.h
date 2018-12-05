#pragma once

#include <memory>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class Stmt;

class LexicalTree {
public:
    virtual ~LexicalTree();

    std::string const &getName() const;

    void setName(std::string const &name);

    bool isModule() const;

    LexicalTree &getParent() const;

    bool hasParent() const;

    void setParent(std::shared_ptr<LexicalTree> const &parent);

    bool hasSlot(std::string const &name) const;

    void declareSlot(std::string const &name);

    size_t getSlotIndex(std::string const &name) const;

    size_t getSlotCount() const;

    size_t getNextCondStmtIndex();

    size_t getNextWhileStmtIndex();

    Stmt const &getStatement() const;

    void setStatement(Stmt const &stmt);

    void addChild(LexicalTree &child);

    LexicalTree &createChild();
};

} // namespace llvmPy
#endif // __cplusplus
