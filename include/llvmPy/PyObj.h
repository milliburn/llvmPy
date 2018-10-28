#pragma once

#include <llvmPy/Typed.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class Type : long {
    None = 0,
    Int = 1,
    Dec = 2,
    Bool = 3,
    Module = 10,
    Func = 11,
};

class PyObj : public Typed<Type> {
public:
    PyObj(Type type) : Typed(type) {}
};

class PyNone : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(Type::None);
    }

    PyNone() : PyObj(Type::None) {}

    static PyNone const &get();
};

class PyInt : public PyObj {
public:
    PyInt(int64_t value) : PyObj(Type::Int), value(value) {}
    int64_t getValue() const { return value; }

private:
    int64_t value;
};

class PyDec : public PyObj {
public:
    PyDec(double value) : PyObj(Type::Dec), value(value) {}
    double getValue() const { return value; }

private:
    double value;
};

class PyBool : public PyObj {
public:
    bool getValue() const;
};

class PyModule : public PyObj {
public:
    std::string getName() const;
};

class PyFunc : public PyObj {
public:
    std::string getName() const;
};

} // namespace llvmPy
#endif // __cplusplus
