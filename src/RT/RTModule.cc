#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <llvm/IR/GlobalVariable.h>
#include <string>
#include <llvm/IR/Constants.h>

using namespace llvmPy;

RTModule::RTModule(
        std::string const &name,
        llvm::Module *module,
        Types const &types)
        : ir(*module),
          types(types),
          scope(*this)
{
}

llvm::Value *
RTModule::llvmPy_add() const
{
    return ir.getOrInsertFunction("llvmPy_add", types.llvmPy_add);
}

llvm::Value *
RTModule::llvmPy_sub() const
{
    return ir.getOrInsertFunction("llvmPy_sub", types.llvmPy_sub);
}

llvm::Value *
RTModule::llvmPy_mul() const
{
    return ir.getOrInsertFunction("llvmPy_mul", types.llvmPy_binop);
}

llvm::Value *
RTModule::llvmPy_int() const
{
    return ir.getOrInsertFunction("llvmPy_int", types.llvmPy_int);
}

llvm::Value *
RTModule::llvmPy_none() const
{
    return ir.getOrInsertFunction("llvmPy_none", types.llvmPy_none);
}

llvm::Value *
RTModule::llvmPy_func() const
{
    return ir.getOrInsertFunction("llvmPy_func", types.llvmPy_func);
}

llvm::Value *
RTModule::llvmPy_fchk() const
{
    return ir.getOrInsertFunction("llvmPy_fchk", types.llvmPy_fchk);
}

llvm::Value *
RTModule::llvmPy_print() const
{
    return ir.getOrInsertFunction("llvmPy_print", types.llvmPy_print);
}

llvm::Value *
RTModule::llvmPy_str() const
{
    return ir.getOrInsertFunction("llvmPy_str", types.llvmPy_str);
}

llvm::Value *
RTModule::llvmPy_bool() const
{
    return ir.getOrInsertFunction("llvmPy_bool", types.llvmPy_bool);
}

llvm::Value *
RTModule::llvmPy_lt() const
{
    return ir.getOrInsertFunction("llvmPy_lt", types.llvmPy_lt);
}

llvm::Value *
RTModule::llvmPy_le() const
{
    return ir.getOrInsertFunction("llvmPy_le", types.llvmPy_le);
}

llvm::Value *
RTModule::llvmPy_eq() const
{
    return ir.getOrInsertFunction("llvmPy_eq", types.llvmPy_eq);
}

llvm::Value *
RTModule::llvmPy_ne() const
{
    return ir.getOrInsertFunction("llvmPy_ne", types.llvmPy_ne);
}

llvm::Value *
RTModule::llvmPy_ge() const
{
    return ir.getOrInsertFunction("llvmPy_ge", types.llvmPy_ge);
}

llvm::Value *
RTModule::llvmPy_gt() const
{
    return ir.getOrInsertFunction("llvmPy_gt", types.llvmPy_gt);
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

    if (auto *var = ir.getGlobalVariable(name, true)) {
        return var;
    } else {
        auto *pyint = new PyInt(value);
        auto *pyintAddr = types.getInt64(reinterpret_cast<uint64_t>(pyint));
        auto *pyintPtr = llvm::ConstantExpr::getIntToPtr(pyintAddr, types.Ptr);
        return new llvm::GlobalVariable(
                ir,
                types.Ptr,
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
        auto *addr = types.getInt64(reinterpret_cast<uint64_t>(obj));
        auto *ptr = llvm::ConstantExpr::getIntToPtr(addr, types.Ptr);
        auto *global = new llvm::GlobalVariable(
                ir,
                types.Ptr,
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
    return ir.getOrInsertFunction("llvmPy_truthy", types.llvmPy_truthy);
}

llvm::Value *
RTModule::llvmPy_len() const
{
    return ir.getOrInsertFunction("llvmPy_len", types.llvmPy_len);
}

llvm::GlobalVariable *
RTModule::getOrCreateGlobalExtern(std::string const &name) const
{
    if (auto *var = ir.getGlobalVariable(name)) {
        return var;
    } else {
        return new llvm::GlobalVariable(
                ir,
                types.PyObj,
                true,
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                nullptr,
                name);
    }
}
