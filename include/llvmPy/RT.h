#pragma once
#include <llvmPy/Typed.h>
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class Function;
class Module;
class Value;
} // namespace llvm

namespace llvmPy {

class RT;
class RTAtom;
class RTAny;
class RTScope;
class RTNone;
class RTBool;
class RTString;
class RTDecimal;
class RTClass;
class RTFunc;
class RTObj;
class RTModule;

enum class RTType : long {
    RTAny,
    RTAtom,
    RTBoolAtom,
    RTIntegerAtom,
    RTDecimalAtom,
    RTNoneAtom,
    RTPtrAtom,
    RTScope,
    RTNone,
    RTBool,
    RTString,
    RTDecimal,
    RTClass,
    RTFunc,
    RTObj,
    RTModule,
};

class RTName {
public:
    RTName(std::string name) : name(std::move(name)) {}
    std::string const name;
};

class RTAtom : public Typed<RTType> {
public:
    static bool classof(RTAtom const *x) {
        return x->isTypeBetween(RTType::RTAtom, RTType::RTPtrAtom);
    }

    inline
    RTAtom(bool v)
    : Typed(RTType::RTBoolAtom)
    { atom.boolean = v; }

    inline
    RTAtom(double v)
    : Typed(RTType::RTDecimalAtom)
    { atom.decimal = v; }

    inline
    RTAtom(RTAny *v)
    : Typed(RTType::RTPtrAtom)
    { atom.any = v; }

    union {
        bool boolean;
        long integer;
        double decimal;
        RTAny *any;
    } atom;

protected:
    explicit inline
    RTAtom(RTType type)
    : Typed(type) {}

private:
} __attribute__((packed));

class RTAny : public RTAtom {
public:
    virtual ~RTAny() = default;
    virtual RTAny * access(RTName const &);
    virtual void assign(RTName const &, RTAny *);

protected:
    explicit RTAny(RTType type) : RTAtom(type) {}
};

class RTScope : public RTAny {
public:
    static RTScope &getNullScope();

    RTScope() : RTScope(nullptr) {}

    explicit RTScope(RTScope *parent)
    : RTAny(RTType::RTScope), parent(parent)
    {}

    RTScope * const parent;
    std::unordered_map<std::string, llvm::Value *> slots;
};

class RTNone : public RTAny {
public:
    explicit RTNone() : RTAny(RTType::RTNone) {}
    static RTNone const &getInstance();
};

class RTBool : public RTAny {
public:
    explicit RTBool(bool value)
    : RTAny(RTType::RTBool)
    { atom.boolean = value; }

    inline bool getValue() const { return atom.boolean; }
};

class RTDecimal : public RTAny {
public:
    explicit
    RTDecimal(double value)
    : RTAny(RTType::RTDecimal)
    { atom.decimal = value; }

    inline double getValue() const { return atom.decimal; }
};

class RTClass : public RTAny {
public:
    explicit RTClass() : RTAny(RTType::RTClass) {}
    std::string name = "";
    RTObj * init();
};

class RTObj : public RTAny {
public:
    explicit RTObj(RTClass &cls)
        : RTAny(RTType::RTObj),
          cls(cls) {}

    RTClass &cls;
    std::unordered_map<std::string, RTObj *> members;
};

class RTFunc : public RTAny {
public:
    static bool classof(RTAtom const *atom) {
        return atom->isType(RTType::RTFunc);
    }

    RTFunc(std::string name, RTScope &scope, llvm::Function *ir)
        : RTAny(RTType::RTFunc),
          name(name),
          scope(scope),
          ir(ir) {}
    std::string name;
    RTScope &scope;
    llvm::Function *ir;
};

class RTModule : public RTAny {
public:
    RTModule(
            std::string const &name,
            RTScope &scope,
            RTFunc &func,
            llvm::Module &ir)
    : RTAny(RTType::RTModule),
      scope(scope),
      func(func),
      ir(ir),
      name(name) {}

    RTScope &scope;
    RTFunc &func;
    llvm::Module &ir;
    std::string const &name;
};

class RT {
public:
    std::unordered_map<std::string, RTModule *> modules;
};

} // namespace llvmPy
#endif // __cplusplus

