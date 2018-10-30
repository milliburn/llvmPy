#pragma once

#include <llvmPy/Typed.h>
#include <string>

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

private:
    int64_t value;
};

class PyFunc : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Func);
    }

    PyFunc(RTFunc &func) : PyObj(PyObjType::Func), func(func) {}

public:
    RTFunc &getFunc() const { return func; }

private:
    RTFunc &func;
};

} // namespace llvmPy
#endif // __cplusplus
