#pragma once
#ifdef __cplusplus
namespace llvmPy {

struct Frame;
class PyObj;

/**
 * The Call Frame Descriptor.
 */
struct Call {
    Frame *frame;
    void *label;
    PyObj *self;

    Call(Frame *frame, void *label, PyObj *self)
    : frame(frame), label(label), self(self)
    {}
};

} // namespace llvmPy
#endif // __cplusplus

