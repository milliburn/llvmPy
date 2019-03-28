#include "PythonImpl.h"
#include <llvm/Support/raw_os_ostream.h>
using namespace llvmPy;

PythonImpl::PythonImpl()
    : _emitter(_compiler),
      _runtime(_compiler),
      _emitterPhase(_emitter),
      _runtimePhase(_runtime),
      _argv(),
      _implOptions(),
      _in(&std::cin),
      _out(&std::cout),
      _err(&std::cerr)
{
}

std::istream &
PythonImpl::getInputStream() const
{
    return *_in;
}

std::ostream &
PythonImpl::getOutputStream() const
{
    return *_out;
}

std::ostream &
PythonImpl::getErrorStream() const
{
    return *_err;
}

void
PythonImpl::setArgv(std::vector<std::string> const &argv)
{
    _argv = argv;
}

void
PythonImpl::addImplOption(std::string const &option)
{
    _implOptions.insert(option);
}

int
PythonImpl::run(
        std::istream &input,
        std::string const &filename)
{
    auto tokens = _lexerPhase.run(input);

    if (hasImplOption("phase=lexer")) {
        printLexerOutput(tokens);
        return 0;
    }

    auto statement = _parserPhase.run(tokens);

    if (hasImplOption("phase=parser")) {
        statement->toStream(getOutputStream());
        return 0;
    }

    _delambdafyRule.run(*statement);

    if (hasImplOption("phase=delambdafy")) {
        statement->toStream(getOutputStream());
        return 0;
    }

    auto module = _emitterPhase.run(*statement);

    if (hasImplOption("phase=ir")) {
        llvm::raw_os_ostream os(getOutputStream());
        module->getModule().print(os, nullptr);
        return 0;
    }

    auto exitcode = _runtimePhase.run(*module);
    return exitcode;
}

bool
PythonImpl::hasImplOption(std::string const &option) const
{
    return _implOptions.count(option) > 0;
}

void
PythonImpl::printLexerOutput(std::vector<Token> const &tokens) const
{
    int iTokenOnLine = 0;
    auto &stream = getOutputStream();

    for (auto const &token : tokens) {
        if (iTokenOnLine > 0
            && token.getTokenType() != tok_eol
            && token.getTokenType() != tok_eof) {
            stream << ' ';
        }

        if (token.getTokenType() == tok_eof) {
            // Right now EOF doesn't have a canonical representation.
            stream << ">EOF";
        } else {
            token.toStream(stream);
        }

        iTokenOnLine += 1;

        if (token.getTokenType() == tok_eol) {
            iTokenOnLine = 0;
        }
    }
}
