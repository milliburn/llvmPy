#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/Target/TargetMachine.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#pragma GCC diagnostic pop

#include <memory>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

using TObjectLayer = llvm::orc::RTDyldObjectLinkingLayer;
using TCompileLayer = llvm::orc::IRCompileLayer<
        TObjectLayer, llvm::orc::SimpleCompiler>;

class CompilerImpl {
public:
    explicit CompilerImpl();

public:
    llvm::DataLayout const &getDataLayout() const { return dataLayout; }
    llvm::TargetMachine &getTargetMachine() const { return *targetMachine; }
    llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> module);
    llvm::JITSymbol findSymbol(std::string const &name);
    llvm::JITSymbol findSymbol(std::string const &name, bool mangle);

private:
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    llvm::DataLayout const dataLayout;
    llvm::orc::ExecutionSession executionSession;
    std::shared_ptr<llvm::orc::SymbolResolver> symbolResolver;
    TObjectLayer objectLayer;
    TCompileLayer compileLayer;
    std::vector<llvm::orc::VModuleKey> moduleKeys;

    std::shared_ptr<llvm::orc::SymbolResolver> createSymbolResolver();
    TObjectLayer::ResourcesGetter createResourcesGetter() const;
    std::string mangleSymbol(std::string const &symbol) const;
};

} // namespace llvmPy
#endif // __cplusplus
