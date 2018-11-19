#pragma once
#ifdef __cplusplus
namespace llvmPy {

class PyObj;

struct Frame {
    static constexpr int SelfIndex = 0;
    static constexpr int OuterIndex = 1;
    static constexpr int VarsIndex = 2;

    Frame *self;
    Frame *outer;
    PyObj *vars[];
};

} // namespace llvmPy
#endif // __cplusplus
