#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
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
: ir(*module), types(types), scope(*this)
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

RTFunc::RTFunc(
        llvm::Function &func,
        RTScope &scope)
: func(func),
  scope(scope)
{
}

