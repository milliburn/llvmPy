#pragma once
#include <llvmPy.h>
#include <llvmPy/Compiler/AllPhases.h>
#include <string>
#include <iostream>
#include <string>
#include <istream>
#include <vector>
#include <unordered_set>

#ifdef __cplusplus
namespace llvmPy {

class PythonImpl {
public:
    PythonImpl();
    ~PythonImpl() = default;

    std::istream &getInputStream() const;
    std::ostream &getOutputStream() const;
    std::ostream &getErrorStream() const;

    void setArgv(std::vector<std::string> const &argv);
    void addImplOption(std::string const &option);

    int run(
            std::istream &input,
            std::string const &filename);

private:
    bool hasImplOption(std::string const &) const;

    Compiler _compiler;
    Emitter _emitter;
    RT _runtime;

    LexerPhase _lexerPhase;
    ParserPhase _parserPhase;
    EmitterPhase _emitterPhase;
    RuntimePhase _runtimePhase;

    std::vector<std::string> _argv;
    std::unordered_set<std::string> _implOptions;

    std::istream *_in;
    std::ostream *_out;
    std::ostream *_err;

    void printLexerOutput(std::vector<Token> const &tokens) const;
};

} // namespace llvmPy
#endif // __cplusplus
