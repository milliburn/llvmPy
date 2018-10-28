#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>

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
