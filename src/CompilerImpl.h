#pragma once
#include <memory>
#include <llvm/Target/TargetMachine.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
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

private:
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    llvm::DataLayout const dataLayout;
    llvm::orc::ExecutionSession executionSession;
    TObjectLayer objectLayer;
    TCompileLayer compileLayer;
    std::vector<llvm::orc::VModuleKey> moduleKeys;
};

} // namespace llvmPy
#endif // __cplusplus
