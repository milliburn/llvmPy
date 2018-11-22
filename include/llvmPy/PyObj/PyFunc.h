#pragma once

#include "PyObj.h"
#include <llvmPy/RT/Call.h>
#include <llvmPy/RT/Frame.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class PyFuncType {
    LibraryFunction,    ///< no frame
    LibraryMethod,      ///< self-argument passed through call frame
    UserFunction,       ///< frame pointer directly in call frame
    UserMethod,         ///< self-argument passed as first parameter
};

std::ostream &operator<< (std::ostream &, PyFuncType const &);

class PyFunc : public PyObj {
public:
    static PyFunc *newLibraryFunction(void *label);
    static PyFunc *newLibraryMethod(void *label, PyObj *obj);
    static PyFunc *newUserFunction(void *label, Frame *frame);
    static PyFunc *newUserMethod(void *label, Frame *frame, PyObj *obj);

    PyFunc(Frame *frame, void *label);

    PyFunc(PyFunc const &copy) noexcept;

    PyFunc(PyFunc &&move) noexcept;

    std::string py__str__() override;

    PyFuncType getType() const;

    Frame *getFrame() const;

    void *getLabel() const;

    PyObj *getSelf() const;

    Call const &getCallFrame() const;

    bool isBound() const;

    bool isMethod() const;

    PyFunc *bind(PyObj &self) const;

private:
    PyFunc(PyFuncType, void *, Frame *, PyObj *);

    PyFuncType const _type;

    Call const _call;
};

} // namespace llvmPy
#endif // __cplusplus
