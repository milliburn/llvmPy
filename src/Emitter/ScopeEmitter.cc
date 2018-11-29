#include "ScopeEmitter.h"
#include <llvmPy/Compiler.h>
using namespace llvmPy;

ScopeEmitter::ScopeEmitter(
        Compiler &compiler,
        ModuleEmitter const &module) noexcept
: _ctx(compiler.getContext()),
  _dl(compiler.getDataLayout()),
  _module(module),
  _ir(_ctx)
{

}

ScopeEmitter::~ScopeEmitter() = default;
