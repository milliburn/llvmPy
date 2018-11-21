#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#pragma GCC diagnostic pop

#include <llvmPy/RT.h>
#include <llvmPy/Instr.h>
#include <llvmPy/Compiler.h>
#include <string>
using namespace llvmPy;

RT::RT(Compiler &compiler)
: _compiler(compiler)
{
}

void
RT::import(RTModule &mod)
{
    std::unique_ptr<llvm::Module> ptr(&mod.getModule());
    _compiler.addAndRunModule(std::move(ptr));
}

