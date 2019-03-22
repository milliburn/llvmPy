#pragma once
#include <llvmPy.h>
#include <llvmPy/Compiler/AllPhases.h>
#include <string>
#include <istream>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

class PythonImpl {
public:
    PythonImpl();
    ~PythonImpl() = default;

    void setArgv(std::vector<std::string> const &argv);

    int run(
            std::istream &input,
            std::string const &filename);

private:
    Compiler _compiler;
    Emitter _emitter;
    RT _runtime;

    LexerPhase _lexerPhase;
    ParserPhase _parserPhase;
    EmitterPhase _emitterPhase;
    RuntimePhase _runtimePhase;

    std::vector<std::string> _argv;

    std::istream *_in;
    std::ostream *_out;
    std::ostream *_err;
};

} // namespace llvmPy
#endif // __cplusplus
