#pragma once
#include <string>
#include <llvmPy/Support/Testing.h>

#ifdef __cplusplus

namespace llvmPy {

class Scope {
public:
    Scope();

    explicit Scope(Scope &parent);

    virtual ~Scope();

    bool hasParent() const;

    __mock_virtual Scope &getParent() const;

    virtual size_t getSlotCount() const = 0;

private:
    Scope * const parent;
};

} // namespace llvmPy
#endif // __cplusplus

