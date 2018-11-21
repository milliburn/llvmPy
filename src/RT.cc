#include <llvmPy/RT.h>
#include <llvm/IR/Module.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <llvm/IR/GlobalVariable.h>
#include <string>
#include <llvm/IR/Constants.h>
using namespace llvmPy;

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

