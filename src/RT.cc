#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
#include <string>

using namespace llvmPy;

RTModule::RTModule(
        std::string const &name,
        llvm::Module *module,
        Types const &types,
        llvm::Function *func)
: ir(*module), types(types), func(*func)
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
RTModule::llvmPy_callN(int N) const
{
    return ir.getOrInsertFunction(
            "llvmPy_call" + std::to_string(N),
            types.llvmPy_callN[N]);
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
