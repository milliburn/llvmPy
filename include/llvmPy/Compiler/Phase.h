#pragma once

#ifdef __cplusplus
namespace llvmPy {

template<typename T>
class Phase {
public:
    virtual ~Phase() = default;
    virtual void run(T &) = 0;
};

} // namespace llvmPy
#endif // __cplusplus
