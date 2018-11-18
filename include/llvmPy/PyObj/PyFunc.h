#pragma once

#include "PyObj.h"
#include <llvmPy/RT/Frame.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyFunc : public PyObj {
public:
    PyFunc(FrameN *frame, void *label);

    std::string py__str__() override;

    FrameN *getFrame() const;

    void *getLabel() const;

private:
    FrameN *frame_;
    void * const label_;
};

} // namespace llvmPy
#endif // __cplusplus
