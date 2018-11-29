#include "ModuleEmitter.h"
#include <llvmPy/Compiler.h>
using namespace llvmPy;

ModuleEmitter::ModuleEmitter(Compiler &compiler) noexcept
: _compiler(compiler),
  _types(compiler.getContext(), compiler.getDataLayout())
{
}

ModuleEmitter::~ModuleEmitter() = default;
