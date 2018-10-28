#pragma once
#include <llvmPy/RTAtom.h>
#include <llvmPy/RTObj.h>
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class Function;
class LLVMContext;
class Value;
} // namespace llvm

namespace llvmPy {

class Types;

class RTScope {
public:
    RTScope() : RTScope(nullptr) {}

    explicit inline
    RTScope(RTScope *parent) : parent(parent) {}

    RTScope * const parent;
    std::unordered_map<std::string, llvm::Value *> slots;
};

class RTModule {
public:
    RTModule(
            std::string const &name,
            llvm::Module *module,
            Types &types,
            llvm::Function *func);

public:
    llvm::Function &getFunction() const { return func; }
    llvm::Module &getModule() const { return ir; }
    RTScope &getScope() { return scope; }

public:
    llvm::Value *llvmPy_add() const;
    llvm::Value *llvmPy_int() const;

private:
    llvm::Module &ir;
    Types &types;
    llvm::Function &func;
    RTScope scope;
};

class RT {
public:
    std::unordered_map<std::string, RTModuleObj *> modules;
};

} // namespace llvmPy
#endif // __cplusplus

