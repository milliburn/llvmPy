#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#pragma GCC diagnostic pop

#include <llvmPy/RT.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <string>
#include <llvmPy/RT/RTModule.h>

using namespace llvmPy;

RTModule::RTModule(
        std::string const &name,
        llvm::Module *module,
        Types const &types)
        : _ir(*module),
          _types(types),
          _scope(*this)
{
}

llvm::Value *
RTModule::llvmPy_add() const
{
    return _ir.getOrInsertFunction("llvmPy_add", _types.llvmPy_add);
}

llvm::Value *
RTModule::llvmPy_sub() const
{
    return _ir.getOrInsertFunction("llvmPy_sub", _types.llvmPy_sub);
}

llvm::Value *
RTModule::llvmPy_mul() const
{
    return _ir.getOrInsertFunction("llvmPy_mul", _types.llvmPy_binop);
}

llvm::Value *
RTModule::llvmPy_int() const
{
    return _ir.getOrInsertFunction("llvmPy_int", _types.llvmPy_int);
}

llvm::Value *
RTModule::llvmPy_none() const
{
    return _ir.getOrInsertFunction("llvmPy_none", _types.llvmPy_none);
}

llvm::Value *
RTModule::llvmPy_func() const
{
    return _ir.getOrInsertFunction("llvmPy_func", _types.llvmPy_func);
}

llvm::Value *
RTModule::llvmPy_fchk() const
{
    return _ir.getOrInsertFunction("llvmPy_fchk", _types.llvmPy_fchk);
}

llvm::Value *
RTModule::llvmPy_print() const
{
    return _ir.getOrInsertFunction("llvmPy_print", _types.llvmPy_print);
}

llvm::Value *
RTModule::llvmPy_str() const
{
    return _ir.getOrInsertFunction("llvmPy_str", _types.llvmPy_str);
}

llvm::Value *
RTModule::llvmPy_bool() const
{
    return _ir.getOrInsertFunction("llvmPy_bool", _types.llvmPy_bool);
}

llvm::Value *
RTModule::llvmPy_lt() const
{
    return _ir.getOrInsertFunction("llvmPy_lt", _types.llvmPy_lt);
}

llvm::Value *
RTModule::llvmPy_le() const
{
    return _ir.getOrInsertFunction("llvmPy_le", _types.llvmPy_le);
}

llvm::Value *
RTModule::llvmPy_eq() const
{
    return _ir.getOrInsertFunction("llvmPy_eq", _types.llvmPy_eq);
}

llvm::Value *
RTModule::llvmPy_ne() const
{
    return _ir.getOrInsertFunction("llvmPy_ne", _types.llvmPy_ne);
}

llvm::Value *
RTModule::llvmPy_ge() const
{
    return _ir.getOrInsertFunction("llvmPy_ge", _types.llvmPy_ge);
}

llvm::Value *
RTModule::llvmPy_gt() const
{
    return _ir.getOrInsertFunction("llvmPy_gt", _types.llvmPy_gt);
}

llvm::GlobalVariable *
RTModule::llvmPy_None() const
{
    return getOrCreateGlobalExtern("llvmPy_None");
}

llvm::GlobalVariable *
RTModule::llvmPy_True() const
{
    return getOrCreateGlobalExtern("llvmPy_True");
}

llvm::GlobalVariable *
RTModule::llvmPy_False() const
{
    return getOrCreateGlobalExtern("llvmPy_False");
}

llvm::GlobalVariable *
RTModule::llvmPy_PyInt(int64_t value) const
{
    std::string sign = value < 0 ? "_" : "";
    std::string name = "PyInt." + sign + std::to_string(abs(value));

    if (auto *var = _ir.getGlobalVariable(name, true)) {
        return var;
    } else {
        auto *pyint = new PyInt(value);
        auto *pyintAddr = _types.getInt64(reinterpret_cast<int64_t>(pyint));
        auto *pyintPtr = llvm::ConstantExpr::getIntToPtr(pyintAddr, _types.Ptr);
        return new llvm::GlobalVariable(
                _ir,
                _types.Ptr,
                true,
                llvm::GlobalVariable::LinkageTypes::PrivateLinkage,
                pyintPtr,
                name);
    }
}

llvm::GlobalVariable *
RTModule::llvmPy_PyStr(std::string const &value)
{
    auto var = strings_.find(value);
    if (var != strings_.end()) {
        return var->second;
    } else {
        auto *obj = new PyStr(value);
        auto *addr = _types.getInt64(reinterpret_cast<int64_t>(obj));
        auto *ptr = llvm::ConstantExpr::getIntToPtr(addr, _types.Ptr);
        auto *global = new llvm::GlobalVariable(
                _ir,
                _types.Ptr,
                true,
                llvm::GlobalVariable::LinkageTypes::PrivateLinkage,
                ptr,
                "PyStr." + std::to_string(strings_.size()));
        strings_[value] = global;
        return global;
    }
}

llvm::Value *
RTModule::llvmPy_truthy() const
{
    return _ir.getOrInsertFunction("llvmPy_truthy", _types.llvmPy_truthy);
}

llvm::Value *
RTModule::llvmPy_len() const
{
    return _ir.getOrInsertFunction("llvmPy_len", _types.llvmPy_len);
}

llvm::Value *
RTModule::llvmPy_getattr() const
{
    return _ir.getOrInsertFunction("llvmPy_getattr", _types.llvmPy_getattr);
}

llvm::GlobalVariable *
RTModule::getOrCreateGlobalExtern(std::string const &name) const
{
    if (auto *var = _ir.getGlobalVariable(name)) {
        return var;
    } else {
        return new llvm::GlobalVariable(
                _ir,
                _types.PyObj,
                true,
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                nullptr,
                name);
    }
}
