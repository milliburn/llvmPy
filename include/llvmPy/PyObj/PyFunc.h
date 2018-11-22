#pragma once

#include "PyObj.h"
#include <llvmPy/RT/Call.h>
#include <llvmPy/RT/Frame.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class PyFuncType {
    LibraryFunction,
    LibraryMethod,
    UserFunction,
    UserMethod,
};

std::ostream &operator<< (std::ostream &, PyFuncType const &);

class PyFunc : public PyObj {
public:
    static PyFunc createLibraryFunction(void *label);
    static PyFunc createLibraryMethod(void *label, PyObj *obj);
    static PyFunc createUserFunction(void *label, Frame *frame);
    static PyFunc createUserMethod(void *label, Frame *frame, PyObj *obj);

    PyFunc(Frame *frame, void *label);

    PyFunc(PyFunc const &copy) noexcept;

    PyFunc(PyFunc &&move) noexcept;

    std::string py__str__() override;

    PyFuncType getType() const;

    Frame *getFrame() const;

    void *getLabel() const;

    Call const &getCallFrame() const;

    bool isBound() const;

private:
    PyFunc(PyFuncType, void *, Frame *, PyObj *);

    PyFuncType const _type;

    void * const _label;

    Call const _call;
};

} // namespace llvmPy
#endif // __cplusplus
