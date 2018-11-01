#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class Function;
class LLVMContext;
class Module;
class Value;
} // namespace llvm

namespace llvmPy {

class Types;
class RTModule;
class RTScope;
class RTFunc;
class RTFrame;
class PyObj;

class RTScope {
public:
    RTScope(RTModule &module,
            RTScope &parent,
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

    explicit RTScope(RTModule &module);

public:
    RTScope *createDerived(
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

    std::unordered_map<std::string, llvm::Value *> slots;

public:
    RTModule &getModule() const { return module; }
    bool hasParent() const { return parent != nullptr; }
    RTScope &getParent() const;
    llvm::Value *getOuterFramePtr() const { return outerFramePtr; }
    llvm::Value *getInnerFramePtr() const { return innerFramePtr; }

private:
    RTModule &module;
    RTScope * const parent;
    llvm::Value * const outerFramePtr;
    llvm::Value * const innerFramePtr;
};

class RTModule {
public:
    RTModule(
            std::string const &name,
            llvm::Module *module,
            Types const &types);

public:
    RTFunc &getBody() const { return *func; }
    void setBody(RTFunc *func) { this->func = func; }
    llvm::Module &getModule() const { return ir; }
    RTScope &getScope() { return scope; }

public:
    llvm::Value *llvmPy_add() const;
    llvm::Value *llvmPy_int() const;
    llvm::Value *llvmPy_none() const;
    llvm::Value *llvmPy_func() const;
    llvm::Value *llvmPy_fchk() const;

private:
    llvm::Module &ir;
    Types const &types;
    RTFunc *func;
    RTScope scope;
};

class RTFunc {
public:
    RTFunc(llvm::Function &func, RTScope &scope);

public:
    llvm::Function &getFunction() const { return func; }
    RTScope &getScope() const { return scope; }

private:
    llvm::Function &func;
    RTScope &scope;
};

/** The frame of a function call. */
class RTFrame {
public:
    explicit RTFrame(RTFunc &func) : func(func) {};

public:
    RTFunc &getFunc() const { return func; }

private:
    RTFunc &func;
    std::vector<PyObj *> values;
};

class RT {
public:
};

} // namespace llvmPy
#endif // __cplusplus

