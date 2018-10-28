#pragma once
#include <llvmPy/Typed.h>
#include <string>

#ifdef __cplusplus
namespace llvm {
class Function;
class Module;
} // namespace llvm

namespace llvmPy {

class RTScope;

enum class RTObjType : long {
    Obj = 0,
    String = 1,
    Func = 2,
    Module = 3,
    Class = 4,
    IntLiteral = 5,
};

class RTObj : public Typed<RTObjType> {
public:
    static bool classof(RTObj const *x) {
        return x->isType(RTObjType::Obj);
    }

    inline RTObj() : Typed(RTObjType::Obj) {}

    inline bool isReadOnly() const { return isReadOnly_; }
    // virtual RTObj * access(std::string const &);
    // virtual void assign(std::string const &, RTObj *);

protected:
    bool isReadOnly_ = false;
    explicit inline RTObj(RTObjType t) : Typed(t) {}
};

class RTObj_ : public Typed<RTObjType> {
public:
    explicit RTObj_(RTObjType type) : Typed(type) {}
};

class RTIntLiteral : public RTObj_ {
public:
    explicit RTIntLiteral(long value) : RTObj_(RTObjType::IntLiteral), value(value) {}
    long const value;
};

class RTFuncObj : public RTObj {
public:
    static bool classof(RTObj const *x) {
        return x->isType(RTObjType::Func);
    }

    RTFuncObj(std::string name, RTScope *scope, llvm::Function *ir)
    : RTObj(RTObjType::Func),
      name(name),
      scope(scope),
      ir(ir) {}

    std::string name;
    RTScope * const scope;
    llvm::Function * const ir;
};

class RTModuleObj : public RTObj {
public:
    static bool classof(RTObj const *x) {
        return x->isType(RTObjType::Module);
    }

    RTModuleObj(
            std::string const &name,
            RTScope *scope,
            RTFuncObj *func,
            llvm::Module *ir)
    : RTObj(RTObjType::Module),
      scope(scope),
      func(func),
      ir(ir),
      name(name) {}

    RTScope * const scope;
    RTFuncObj * const func;
    llvm::Module * const ir;
    std::string name;
};

} // namespace llvmPy
#endif // __cplusplus
