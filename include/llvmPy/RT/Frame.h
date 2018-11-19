#pragma once

#include <stddef.h>

#ifdef __cplusplus
namespace llvmPy {

class PyObj;

struct Frame {
    static constexpr int SelfIndex = 0;
    static constexpr int OuterIndex = 1;
    static constexpr int CountIndex = 2;
    static constexpr int VarsIndex = 3;

    Frame *self;
    Frame *outer;
    size_t count;
    PyObj *vars[];
};

} // namespace llvmPy
#endif // __cplusplus
