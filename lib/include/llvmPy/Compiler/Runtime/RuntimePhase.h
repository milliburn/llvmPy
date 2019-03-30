#pragma once
#include <llvmPy/Compiler/Phase.h>
#include <llvmPy/RT.h>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class RuntimePhase : public Phase<RTModule &, int> {
public:
    explicit RuntimePhase(RT &rt);
    int run(RTModule &) override;

private:
    RT &_rt;
};

} // namespace llvmPy
#endif // __cplusplus
