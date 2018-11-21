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

    MOCK_VIRTUAL bool hasParent() const;

    MOCK_VIRTUAL Scope &getParent() const;

    virtual size_t getSlotCount() const = 0;

private:
    Scope * const parent_;
};

} // namespace llvmPy
#endif // __cplusplus

