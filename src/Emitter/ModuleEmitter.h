#pragma once

#include <llvmPy/Instr.h>
#include <memory>
#include <string>
#include "ScopeEmitter.h"

#ifdef __cplusplus
namespace llvm {
class Module;
} // namespace llvm

namespace llvmPy {

class Compiler;
class Stmt;
class Types;

class ModuleEmitter : public ScopeEmitter {
public:
    explicit ModuleEmitter(Compiler &compiler) noexcept;

    virtual ~ModuleEmitter();

    std::unique_ptr<llvm::Module>
    emitModule(
            std::string const &name,
            Stmt const &stmt);

private:
    Compiler &_compiler;
    Types const _types;
};

} // namespace llvmPy
#endif // __cplusplus
