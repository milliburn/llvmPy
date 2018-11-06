#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/RT/Frame.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
using namespace llvmPy;

using TObjectLayer = llvm::orc::RTDyldObjectLinkingLayer;
using TCompileLayer = llvm::orc::IRCompileLayer<
        TObjectLayer, llvm::orc::SimpleCompiler>;

static std::unique_ptr<llvm::TargetMachine>
initTargetMachine() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    return std::make_unique<llvm::TargetMachine>(
            llvm::EngineBuilder().selectTarget());
}

static TObjectLayer::Resources
createMemoryManager(llvm::orc::VModuleKey moduleKey)
{
    return TObjectLayer::Resources {
        std::make_shared<llvm::SectionMemoryManager>(),
    };
}

namespace llvmPy {

class CompilerImpl {
public:
    explicit CompilerImpl();

public:
    llvm::DataLayout const &getDataLayout() const { return dataLayout; }
    llvm::TargetMachine &getTargetMachine() const { return *targetMachine; }

    llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> module);

private:
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    llvm::DataLayout const dataLayout;
    llvm::orc::ExecutionSession executionSession;
    TObjectLayer objectLayer;
    TCompileLayer compileLayer;
};

} // namespace llvmPy

CompilerImpl::CompilerImpl()
: targetMachine(initTargetMachine()),
  dataLayout(targetMachine->createDataLayout()),
  objectLayer(executionSession, createMemoryManager),
  compileLayer(objectLayer, llvm::orc::SimpleCompiler(*targetMachine))
{
}

llvm::orc::VModuleKey
CompilerImpl::addModule(std::unique_ptr<llvm::Module> module)
{
    llvm::orc::VModuleKey moduleKey = executionSession.allocateVModule();
    llvm::cantFail(compileLayer.addModule(moduleKey, std::move(module)));
    return moduleKey;
}

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

PyObj *
Compiler::run(
        llvm::Function *function,
        std::vector<llvm::Value *> const &args)
{
    return nullptr;
}
