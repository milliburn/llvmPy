#include "ModuleEmitter.h"
#include <llvmPy/Compiler.h>
using namespace llvmPy;

ModuleEmitter::ModuleEmitter(Compiler &compiler) noexcept
: ScopeEmitter(compiler, *this),
  _compiler(compiler),
  _types(compiler.getContext(), compiler.getDataLayout())
{
}

ModuleEmitter::~ModuleEmitter() = default;

std::unique_ptr<llvm::Module>
ModuleEmitter::emitModule(
        std::string const &name,
        Stmt const &stmt)
{
    return std::unique_ptr<llvm::Module>();
}
