#include "CompilerImpl.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
using namespace llvmPy;

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
