#include "CompilerImpl.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <algorithm>
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
    moduleKeys.push_back(moduleKey);
    return moduleKey;
}

llvm::JITSymbol
CompilerImpl::findSymbol(std::string const &name)
{
    // Search keys in reverse order.
    for (auto i = moduleKeys.size() - 1; i >= 0; i--){
        auto moduleKey = moduleKeys[i];
        if (auto symbol = compileLayer.findSymbolIn(moduleKey, name, true)){
            return symbol;
        }
    }

    return nullptr;
}
