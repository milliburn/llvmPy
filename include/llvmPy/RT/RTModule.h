#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <llvmPy/RT/Frame.h>
#include <llvmPy/RT/RTScope.h>
#include <unordered_map>

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
class PyObj;

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
    llvm::Value *llvmPy_len() const;

    llvm::GlobalVariable *llvmPy_None() const;
    llvm::GlobalVariable *llvmPy_True() const;
    llvm::GlobalVariable *llvmPy_False() const;

    llvm::GlobalVariable *llvmPy_PyInt(int64_t value) const;
    llvm::GlobalVariable *llvmPy_PyStr(std::string const &value);

private:
    llvm::Module &ir;
    Types const &types;
    RTScope scope;

    llvm::GlobalVariable *getOrCreateGlobalExtern(std::string const &name) const;
    std::unordered_map<std::string, llvm::GlobalVariable *> strings_;
};

} // namespace llvmPy
#endif // __cplusplus

