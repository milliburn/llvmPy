#pragma once

#include <llvmPy/Typed.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class PyObjType : long {
    None = 0,
    Int = 1,
};

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

    static PyNone const &get();
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

} // namespace llvmPy
#endif // __cplusplus
