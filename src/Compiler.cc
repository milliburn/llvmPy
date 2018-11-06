#include <llvmPy/Compiler.h>
#include <llvmPy/PyObj.h>
#include "CompilerImpl.h"
using namespace llvmPy;

Compiler::Compiler() noexcept
: impl(std::make_unique<CompilerImpl>())
{
}

Compiler::~Compiler() = default;

llvm::DataLayout const &
Compiler::getDataLayout() const
{
    return impl->getDataLayout();
}

void
Compiler::addAndRunModule(std::unique_ptr<llvm::Module> module)
{
    impl->addModule(std::move(module));
    llvm::JITSymbol moduleBody = impl->findSymbol("__body__");
    auto expectedTargetAddress = moduleBody.getAddress();
    llvm::JITTargetAddress targetAddress = expectedTargetAddress.get();
    auto moduleBodyPtr = reinterpret_cast<PyObj*(*)(void *)>(targetAddress);
    moduleBodyPtr(nullptr);
}
