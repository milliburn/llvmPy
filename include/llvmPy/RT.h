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
            Types const &types,
            llvm::Function *func);

public:
    llvm::Function &getFunction() const { return func; }
    llvm::Module &getModule() const { return ir; }
    RTScope &getScope() { return scope; }

public:
    llvm::Value *llvmPy_add() const;
    llvm::Value *llvmPy_int() const;
    llvm::Value *llvmPy_none() const;
    llvm::Value *llvmPy_callN(int N) const;
    llvm::Value *llvmPy_func() const;
    llvm::Value *llvmPy_fchk() const;

private:
    llvm::Module &ir;
    Types const &types;
    llvm::Function &func;
    RTScope scope;
};

class RTFunc {
public:
    RTFunc(llvm::Function *func, RTScope *scope) : func(*func), scope(*scope) {}

public:
    llvm::Function &getFunction() const { return func; }
    RTScope &getScope() const { return scope; }

private:
    llvm::Function &func;
    RTScope &scope;
};

class RT {
public:
};

} // namespace llvmPy
#endif // __cplusplus

