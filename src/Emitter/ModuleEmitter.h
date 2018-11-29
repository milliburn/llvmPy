#pragma once

#include <llvmPy/Instr.h>

#ifdef __cplusplus
namespace llvmPy {

class Compiler;
class Types;

class ModuleEmitter {
public:
    explicit ModuleEmitter(Compiler &compiler) noexcept;
    virtual ~ModuleEmitter();

private:
    Compiler &_compiler;
    Types const _types;

};

} // namespace llvmPy
#endif // __cplusplus
