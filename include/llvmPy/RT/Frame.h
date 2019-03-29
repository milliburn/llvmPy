#pragma once

#include <stddef.h>

#ifdef __cplusplus
namespace llvmPy {

class PyObj;

struct Frame {
public:
    static constexpr int SelfIndex = 0;
    static constexpr int OuterIndex = 1;
    static constexpr int VarsIndex = 2;

    static constexpr size_t getSizeof(size_t N) {
        return sizeof(Frame) + N * sizeof(PyObj *);
    }

public:
    // Ignore -pedantic to allow flexible array member in C++. The actual space
    // taken by the array is defined in the specific stack frame.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    Frame *self;
    Frame *outer;
    PyObj *vars[];
#pragma GCC diagnostic pop

    Frame() noexcept; ///< Constructor used in tests.
    bool isPointingToHeap() const;
    bool isOnHeap() const;
    void moveToHeapFrame(Frame *heapFrame, size_t N);
    void setIsPointingToHeap();
};

} // namespace llvmPy
#endif // __cplusplus
