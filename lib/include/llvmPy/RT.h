#pragma once
#include <llvmPy/RT/Frame.h>
#include <llvmPy/RT/RTModule.h>
#include <llvmPy/RT/RTScope.h>

#ifdef __cplusplus
namespace llvmPy {

class Compiler;
class Types;

class RT {
public:
    explicit RT(Compiler &compiler);
    void import(RTModule &mod);

private:
    Compiler &_compiler;
};

} // namespace llvmPy
#endif // __cplusplus

