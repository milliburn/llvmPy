#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <llvm/IR/GlobalVariable.h>
#include <string>
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

llvm::Value *
RTModule::llvmPy_truthy() const
{
    return ir.getOrInsertFunction("llvmPy_truthy", types.llvmPy_truthy);
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

RTFunc::RTFunc(
        llvm::Function &func,
        RTScope &scope)
: func(func),
  scope(scope)
{
}

RT::RT(Compiler &compiler)
: compiler(compiler)
{
}

void
RT::import(RTModule &mod)
{
    std::unique_ptr<llvm::Module> ptr(&mod.getModule());
    compiler.addAndRunModule(std::move(ptr));
}

