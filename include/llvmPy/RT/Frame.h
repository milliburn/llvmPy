#pragma once
#ifdef __cplusplus
namespace llvmPy {

class PyObj;

template<int N>
class Frame {
    Frame<N> *self;
    Frame<0> *outer;
    PyObj *vars[N];
};

typedef Frame<0> FrameN;

} // namespace llvmPy
#endif // __cplusplus
