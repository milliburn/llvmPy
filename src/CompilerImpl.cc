#include "CompilerImpl.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <algorithm>
#include <sstream>
using namespace llvmPy;

static std::unique_ptr<llvm::TargetMachine>
initTargetMachine() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    return std::unique_ptr<llvm::TargetMachine>(
            llvm::EngineBuilder().selectTarget());
}

CompilerImpl::CompilerImpl()
        : targetMachine(initTargetMachine()),
          dataLayout(targetMachine->createDataLayout()),
          symbolResolver(createSymbolResolver()),
          objectLayer(executionSession, createResourcesGetter()),
          compileLayer(objectLayer, llvm::orc::SimpleCompiler(*targetMachine))
{
    // TODO: What does this accomplish?
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
}

llvm::orc::VModuleKey
CompilerImpl::addModule(std::unique_ptr<llvm::Module> module)
{
    module->setDataLayout(dataLayout);
    llvm::orc::VModuleKey moduleKey = executionSession.allocateVModule();
    llvm::cantFail(compileLayer.addModule(moduleKey, std::move(module)));
    moduleKeys.push_back(moduleKey);
    return moduleKey;
}

llvm::JITSymbol
CompilerImpl::findSymbol(std::string const &name)
{
    if (auto symbol = findSymbol(name, false)) {
        return symbol;
    }

    if (auto symbol = findSymbol(name, true)) {
        return symbol;
    }

    return nullptr;
}

llvm::JITSymbol
CompilerImpl::findSymbol(std::string const &name, bool mangle)
{
    std::string const mangledName = mangle ? mangleSymbol(name) : name;

    // Search keys in reverse order among the modules that have been added.
    for (auto i = moduleKeys.size(); i--;){
        auto moduleKey = moduleKeys[i];
        if (auto symbol =
                compileLayer.findSymbolIn(moduleKey, mangledName, true)) {
            return symbol;
        }
    }

    // Search through the host process.
    if (auto addr =
            llvm::RTDyldMemoryManager::getSymbolAddressInProcess(
                    mangledName)) {
        return llvm::JITSymbol(addr, llvm::JITSymbolFlags::Exported);
    }

    return nullptr;
}

std::shared_ptr<llvm::orc::SymbolResolver>
CompilerImpl::createSymbolResolver()
{
    return llvm::orc::createLegacyLookupResolver(
            executionSession,
            [this](std::string const &name) {
                return objectLayer.findSymbol(name, true);
            },
            [](llvm::Error err) {
                llvm::cantFail(
                        std::move(err),
                        "lookup failed");
            });
}

TObjectLayer::ResourcesGetter
CompilerImpl::createResourcesGetter() const
{
    return [this](llvm::orc::VModuleKey moduleKey) {
        return TObjectLayer::Resources {
                std::make_shared<llvm::SectionMemoryManager>(),
                symbolResolver,
        };
    };
}

std::string
CompilerImpl::mangleSymbol(std::string const &symbol) const
{
    std::string s;
    llvm::raw_string_ostream ss(s);
    llvm::Mangler::getNameWithPrefix(ss, symbol, dataLayout);
    return ss.str();
}
