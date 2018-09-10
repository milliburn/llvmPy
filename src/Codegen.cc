#include <llvmPy/Codegen.h>

llvmPy::Codegen::Codegen()
: ir(ctx)
{

}

llvm::LLVMContext &
llvmPy::Codegen::getContext()
{
    return ctx;
}

llvm::IRBuilder<> &
llvmPy::Codegen::getBuilder()
{
    return ir;
}
