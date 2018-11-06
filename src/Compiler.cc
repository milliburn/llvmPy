#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvmPy/Compiler.h>
#include <llvmPy/RT/Frame.h>
using namespace llvmPy;

typedef PyObj *(*ModuleFunction)(FrameN *);

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

PyObj *
Compiler::run(
        llvm::Function *function,
        std::vector<llvm::Value *> const &args)
{
    ModuleFunction *mf = (ModuleFunction *) ee.getPointerToFunction(function);
    PyObj *rv = (*mf)(nullptr);
    return rv;
}
