#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <llvmPy/RT/Frame.h>

#ifdef __cplusplus
namespace llvm {
class Function;
class LLVMContext;
class Module;
class Value;
class GlobalVariable;
} // namespace llvm

namespace llvmPy {

class Compiler;
class Types;
class RTModule;
class RTScope;
class RTFunc;
class PyObj;

class RTScope {
public:
    struct Slot {
        llvm::Value *value;
        size_t frameIndex;
    };

    RTScope(RTModule &module,
            RTScope &parent,
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

    explicit RTScope(RTModule &module);

public:
    RTScope *createDerived(
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

public:
    RTModule &getModule() const;

    bool hasParent() const;

    RTScope &getParent() const;

    llvm::Value *getOuterFramePtr() const;

    llvm::Value *getInnerFramePtr() const;

    bool hasSlot(std::string const &name) const;

    void declareSlot(std::string const &name);

    llvm::Value *getSlotValue(std::string const &name) const;

    void setSlotValue(std::string const &name, llvm::Value *value);

    size_t getSlotIndex(std::string const &name) const;

private:
    RTModule &module;
    RTScope * const parent;
    llvm::Value * const outerFramePtr;
    llvm::Value * const innerFramePtr;

    std::unordered_map<std::string, Slot> slots;

};

class RTModule {
public:
    RTModule(
            std::string const &name,
            llvm::Module *module,
            Types const &types);

public:
    llvm::Module &getModule() const { return ir; }
    RTScope &getScope() { return scope; }

public:
    llvm::Value *llvmPy_add() const;
    llvm::Value *llvmPy_sub() const;
    llvm::Value *llvmPy_mul() const;
    llvm::Value *llvmPy_int() const;
    llvm::Value *llvmPy_none() const;
    llvm::Value *llvmPy_func() const;
    llvm::Value *llvmPy_fchk() const;
    llvm::Value *llvmPy_print() const;
    llvm::Value *llvmPy_str() const;
    llvm::Value *llvmPy_bool() const;
    llvm::Value *llvmPy_lt() const;
    llvm::Value *llvmPy_le() const;
    llvm::Value *llvmPy_eq() const;
    llvm::Value *llvmPy_ne() const;
    llvm::Value *llvmPy_ge() const;
    llvm::Value *llvmPy_gt() const;
    llvm::Value *llvmPy_truthy() const;

    llvm::GlobalVariable *llvmPy_None() const;
    llvm::GlobalVariable *llvmPy_True() const;
    llvm::GlobalVariable *llvmPy_False() const;

private:
    llvm::Module &ir;
    Types const &types;
    RTScope scope;

    llvm::GlobalVariable *getOrCreateGlobalExtern(std::string const &name) const;
};

class RT {
public:
    explicit RT(Compiler &compiler);
    void import(RTModule &mod);

private:
    Compiler &compiler;
};

} // namespace llvmPy
#endif // __cplusplus

