#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class Function;
class Value;
} // namespace llvm

namespace llvmPy {

class RT;
class RTAtom;
class RTAny;
class RTValue;
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
    RTValue,
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

class RTAtom {
public:
    explicit inline
    RTAtom(bool v)
    : type(RTType::RTBoolAtom)
    { atom.boolean = v; }

    explicit inline
    RTAtom(double v)
    : type(RTType::RTDecimalAtom)
    { atom.decimal = v; }

    explicit inline
    RTAtom(RTAny *v)
    : type(RTType::RTPtrAtom)
    { atom.any = v; }

    RTType getType() const;

    RTType const type;

    union {
        bool boolean;
        long integer;
        double decimal;
        RTAny *any;
    } atom;

protected:
    explicit RTAtom(RTType type) : type(type) {}

private:
} __attribute__((packed));

class RTAny : private RTAtom {
public:
    using RTAtom::getType;

    virtual ~RTAny() = default;
    virtual RTAny * access(RTName const &);
    virtual void assign(RTName const &, RTAny *);

protected:
    using RTAtom::atom;
    explicit RTAny(RTType type) : RTAtom(type) {}
};

class RTScope : public RTAny {
public:
    explicit RTScope() : RTAny(RTType::RTScope) {}

    RTScope *parent = nullptr;
    std::unordered_map<std::string, RTValue *> slots;

    RTValue &addSlot(std::string const &);
};

class RTNone : public RTAny {
public:
    explicit RTNone() : RTAny(RTType::RTNone) {}
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

class RTValue : public RTAny {
public:
    explicit RTValue() : RTAny(RTType::RTValue) {}
    llvm::Value *ir = nullptr;
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

    RTObj * access(RTName) override;
    void assign(RTName, RTAny *) override;
};

class RTFunc : public RTAny {
public:
    RTFunc(
            std::string name,
            llvm::Function *ir)
        : RTAny(RTType::RTFunc),
          name(name),
          ir(ir) {}
    std::string name;
    llvm::Function *ir;
};

class RT {
public:
    std::unordered_map<std::string, RTScope *> modules;
};

} // namespace llvmPy
#endif // __cplusplus

