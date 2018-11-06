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

llvm::TargetMachine &
Compiler::getTargetMachine() const
{
    return impl->getTargetMachine();
}

void
Compiler::addAndRunModule(std::unique_ptr<llvm::Module> module)
{
    impl->addModule(std::move(module));
    llvm::JITSymbol moduleBody = impl->findSymbol("__body__");
    auto moduleBodyPtr = reinterpret_cast<PyObj*(*)(void *)>(
            llvm::cantFail(moduleBody.getAddress()));
    moduleBodyPtr(nullptr);
}
