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
    Frame *frame_;
    void * const label_;
};

} // namespace llvmPy
#endif // __cplusplus
