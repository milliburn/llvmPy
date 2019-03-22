#include "PythonImpl.h"
#include <iostream>
using namespace llvmPy;

PythonImpl::PythonImpl()
    : _emitter(_compiler),
      _runtime(_compiler),
      _emitterPhase(_emitter),
      _runtimePhase(_runtime),
      _in(&std::cin),
      _out(&std::cout),
      _err(&std::cerr)
{
}

void
PythonImpl::setArgv(std::vector<std::string> const &argv)
{
    _argv = argv;
}

int
PythonImpl::run(
        std::istream &input,
        std::string const &filename)
{
    auto tokens = _lexerPhase.run(input);
    auto statement = _parserPhase.run(tokens);
    auto module = _emitterPhase.run(*statement);
    auto exitcode = _runtimePhase.run(*module);
    return exitcode;
}
