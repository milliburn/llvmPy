#pragma once
#include <llvmPy/Compiler/Phase.h>
#include <llvmPy/Token.h>
#include <istream>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

class LexerPhase : public Phase<std::istream, std::vector<Token>> {
public:
    LexerPhase();
    std::vector<Token> run(std::istream &) override;
};

} // namespace llvmPy
#endif // __cplusplus
