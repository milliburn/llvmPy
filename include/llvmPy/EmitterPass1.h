#pragma once

#include <memory>
#include <string>

#ifdef __cplusplus
namespace llvm {
class LLVMContext;
}

namespace llvmPy {

class Compiler;
class ModuleInfo;
class Stmt;

class EmitterPass1 {
public:
    explicit EmitterPass1(Compiler &compiler) noexcept;

    std::unique_ptr<ModuleInfo> run(
            std::string const &name,
            std::shared_ptr<Stmt const> const &stmt);

private:
    llvm::LLVMContext &_llvmContext;
};


} // namespace llvmPy
#endif // __cplusplus
