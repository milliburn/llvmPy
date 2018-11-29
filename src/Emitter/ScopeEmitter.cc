#include "ScopeEmitter.h"
using namespace llvmPy;

ScopeEmitter::ScopeEmitter(
        llvm::DataLayout const &dl,
        llvm::LLVMContext &ctx,
        Types const &types) noexcept
: _ctx(ctx), _dl(dl), _types(types), _ir(_ctx)
{

}

ScopeEmitter::~ScopeEmitter() = default;
