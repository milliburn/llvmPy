#include <llvmPy/Python.h>
#include "PythonImpl.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
using namespace llvmPy;

Python::Python()
    : _impl()
{
}

Python::~Python() = default;

void
Python::start(std::vector<std::string> const &argv)
{
    _impl->setArgv(argv);
}

void
Python::addImplOption(std::string const &option)
{
    _impl->addImplOption(option);
}

int
Python::runStdin()
{
    return _impl->run(std::cin, "<stdin>");
}

int
Python::runCommand(std::istream &command)
{
    return _impl->run(command, "");
}

int
Python::runScript(std::string const &path)
{
    std::ifstream fp;
    fp.open(path, std::ios::in);

    if (fp.fail()) {
        std::stringstream ss;
        ss << "Cannot open file '" << path << "': not found.";
        throw std::runtime_error(ss.str());
    }

    return _impl->run(fp, path);
}
