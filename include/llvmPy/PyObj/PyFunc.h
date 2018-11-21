#pragma once

#include "PyObj.h"
#include <llvmPy/RT/Frame.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyFunc : public PyObj {
public:
    PyFunc(Frame *frame, void *label);

    std::string py__str__() override;

    Frame *getFrame() const;

    void *getLabel() const;

private:
    Frame *_frame;
    void * const _label;
};

} // namespace llvmPy
#endif // __cplusplus
