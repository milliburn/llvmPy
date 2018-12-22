#pragma once

#ifdef __cplusplus
namespace llvmPy {

template<class I, class O>
class Pass {
public:
    using Input = I;
    using Output = O;

    virtual ~Pass() = default;
    virtual O const run(I const &) = 0;
};

} // namespace llvmPy
#endif // __cplusplus
