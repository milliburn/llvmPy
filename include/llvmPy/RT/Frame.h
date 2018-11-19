#pragma once
#ifdef __cplusplus
namespace llvmPy {

class PyObj;

struct Frame {
    Frame *self;
    Frame *outer;
    PyObj *vars[];
};

} // namespace llvmPy
#endif // __cplusplus
