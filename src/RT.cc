#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <string>
using namespace llvmPy;

RTScope::RTScope(
        RTModule &module,
        RTScope &parent,
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
: module(module),
  parent(&parent),
  outerFramePtr(outerFramePtr),
  innerFramePtr(innerFramePtr)
{

}

RTScope::RTScope(RTModule &module)
: module(module),
  parent(nullptr),
  outerFramePtr(nullptr),
  innerFramePtr(nullptr)
{
}

RTScope *
RTScope::createDerived(
        llvm::Value *innerFramePtr,
        llvm::Value *outerFramePtr)
{
    return new RTScope(module, *this, innerFramePtr, outerFramePtr);
}

RTScope &
RTScope::getParent() const
{
    if (!hasParent()) {
        throw "Cannot return parent of top-level RTScope.";
    }

    return *parent;
}

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

