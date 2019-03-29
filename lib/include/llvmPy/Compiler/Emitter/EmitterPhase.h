#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler/Phase.h>
#include <llvmPy/Emitter.h>
#include <llvmPy/RT/RTModule.h>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class EmitterPhase : public Phase<Stmt const, std::unique_ptr<RTModule>> {
public:
    explicit EmitterPhase(Emitter &);
    std::unique_ptr<RTModule> run(Stmt const &) override;

private:
    Emitter &_emitter;
};

} // namespace llvmPy
#endif // __cplusplus
