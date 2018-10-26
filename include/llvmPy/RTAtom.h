#pragma once
#include <llvmPy/Typed.h>

#ifdef __cplusplus
namespace llvmPy {

class RTObj;
class RTFuncObj;

enum class RTAtomType : long {
    None = 0,
    Int = 1,
    Dec = 2,
    Bool = 3,
    Obj = 4,
    FuncObj = 0x100,
    Func0 = FuncObj,
    Func1,
    Func2,
    FuncN = 0x1FF,
};

class RTAtom : public Typed<RTAtomType> {
public:
    static bool classof(RTAtom const *x) {
        return true;
    }

    inline long getIntValue() const { return val.integer; }
    inline double getDecValue() const { return val.decimal; }
    inline bool getBoolValue() const { return val.boolean; }
    inline RTObj * getObjValue() const { return val.obj; }
    inline RTFuncObj * getFuncValue() const { return val.func; }

protected:
    explicit inline RTAtom(RTAtomType type) : Typed(type) {}

    union {
        long integer;
        double decimal;
        bool boolean;
        RTObj *obj;
        RTFuncObj *func;
    } val;
};

class RTNoneAtom : public RTAtom {
public:
    static bool classof(RTAtom const *x) {
        return x->isType(RTAtomType::None);
    }

    explicit inline
    RTNoneAtom()
    : RTAtom(RTAtomType::None)
    {}
};

class RTIntAtom : public RTAtom {
public:
    static bool classof(RTAtom const *x) {
        return x->isType(RTAtomType::Int);
    }

    explicit inline
    RTIntAtom(long x)
    : RTAtom(RTAtomType::Int)
    { val.integer = x; }

    inline long getValue() const { return val.integer; }
};

class RTDecAtom : public RTAtom {
public:
    static bool classof(RTAtom const *x) {
        return x->isType(RTAtomType::Dec);
    }

    explicit inline
    RTDecAtom(double x)
    : RTAtom(RTAtomType::Dec)
    { val.decimal = x; }

    inline double getValue() const { return val.decimal; }
};

class RTBoolAtom : public RTAtom {
public:
    static bool classof(RTAtom const *x) {
        return x->isType(RTAtomType::Bool);
    }

    explicit inline
    RTBoolAtom(bool x)
    : RTAtom(RTAtomType::Bool)
    { val.boolean = x; }

    inline bool getValue() const { return val.boolean; }
};

class RTObjAtom : public RTAtom {
public:
    static bool classof(RTAtom const *x) {
        return x->isType(RTAtomType::Obj);
    }

    explicit inline
    RTObjAtom(RTObj *x)
    : RTAtom(RTAtomType::Obj)
    { val.obj = x; }

    inline RTObj * getValue() const { return val.obj; }
};

} // namespace llvmPy
#endif // __cplusplus
