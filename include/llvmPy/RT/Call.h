#pragma once
#ifdef __cplusplus
namespace llvmPy {

class PyObj;

/**
 * The Call Frame Descriptor.
 */
struct Call {
    void *label;
    PyObj *self;
};

} // namespace llvmPy
#endif // __cplusplus

