#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler/Phase.h>
#include <llvmPy/Token.h>
#include <memory>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

class ParserPhase : public Phase<std::vector<Token>, std::unique_ptr<Stmt>> {
public:
    ParserPhase();
    std::unique_ptr<Stmt> run(std::vector<Token> &) override;
};

} // namespace llvmPy
#endif // __cplusplus
