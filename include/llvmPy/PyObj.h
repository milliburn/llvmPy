#pragma once

#include <llvmPy/Typed.h>
#include <string>
#include <llvmPy/RT/Frame.h>
#include <llvmPy/PyObj/PyObj.h>
#include <llvmPy/PyObj/PyStr.h>
#include <llvmPy/PyObj/PyBool.h>
#include <llvmPy/PyObj/PyInt.h>
#include <llvmPy/PyObj/PyNone.h>

#ifdef __cplusplus
namespace llvmPy {

class RTFunc;

class PyFunc : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Func);
    }

    PyFunc(RTFunc *func, FrameN *frame, void *label)
    : PyObj(PyObjType::Func), func(*func), frame(frame), label(label) {}
    virtual std::string py__str__() override;

public:
    RTFunc &getFunc() const { return func; }
    FrameN &getFrame() const { return *frame; }
    void *getLabel() const { return label; }

private:
    RTFunc &func;
    FrameN *frame;
    void * const label;
};

} // namespace llvmPy
#endif // __cplusplus
