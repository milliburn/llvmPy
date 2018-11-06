#pragma once

#include <llvmPy/Typed.h>
#include <string>
#include <llvmPy/RT/Frame.h>

#ifdef __cplusplus
namespace llvmPy {

class RTFunc;

enum class PyObjType : long {
    None = 0,
    Int = 1,
    Func = 2,
};

/**
 * The base class for all objects accessible on the heap at runtime.
 */
class PyObj : public Typed<PyObjType> {
public:
    PyObj(PyObjType type) : Typed(type) {}
    virtual std::string py__str__();
};

class PyNone : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::None);
    }

    PyNone() : PyObj(PyObjType::None) {}

    static PyNone *get();
};

class PyInt : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Int);
    }

    PyInt(int64_t value) : PyObj(PyObjType::Int), value(value) {}
    int64_t getValue() const { return value; }
    virtual std::string py__str__() override;

private:
    int64_t value;
};

class PyFunc : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Func);
    }

    PyFunc(RTFunc *func, FrameN *frame)
    : PyObj(PyObjType::Func), func(*func), frame(frame) {}

public:
    RTFunc &getFunc() const { return func; }
    FrameN &getFrame() const { return *frame; }

private:
    RTFunc &func;
    FrameN *frame;
};

} // namespace llvmPy
#endif // __cplusplus
