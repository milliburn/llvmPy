#include <llvmPy/Compiler.h>
#include "CompilerImpl.h"
using namespace llvmPy;

Compiler::Compiler() noexcept
: impl(std::make_unique<CompilerImpl>())
{
}

llvm::DataLayout const &
Compiler::getDataLayout() const
{
    return impl->getDataLayout();
}

llvm::TargetMachine &
Compiler::getTargetMachine() const
{
    return impl->getTargetMachine();
}

void
Compiler::run(
        llvm::Function *function,
        std::vector<llvm::Value *> const &args)
{
    return nullptr;
}

void
Compiler::addModule(std::unique_ptr<llvm::Module> module)
{
    impl->addModule(std::move(module));
}

llvm::JITSymbol
Compiler::findSymbol(std::string const &name)
{
    return impl->findSymbol(name);
}
