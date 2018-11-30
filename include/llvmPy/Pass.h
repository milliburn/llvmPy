#pragma once

#include <memory>

#ifdef __cplusplus
namespace llvmPy {

template<class I, class O>
class Pass {
public:
    virtual ~Pass() = default;
    virtual O run(I const &) = 0;
};

} // namespace llvmPy
#endif // __cplusplus
