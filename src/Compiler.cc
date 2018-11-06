#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvmPy/Compiler.h>
using namespace llvmPy;

static llvm::TargetMachine &
initTargetMachine() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    return *llvm::EngineBuilder().selectTarget();
}

Compiler::Compiler() noexcept
: tm(initTargetMachine()),
  dl(tm.createDataLayout()),
  ee(*llvm::EngineBuilder().create(&tm))
{
}
