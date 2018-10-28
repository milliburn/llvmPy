#include <llvmPy/SyntaxError.h>
using namespace llvmPy;
using std::string;
using std::runtime_error;

EmitterError::EmitterError(std::string const & what)
: runtime_error(what)
{
}

ParserError::ParserError(std::string const & what)
: runtime_error(what)
{
}

SyntaxError::SyntaxError(std::string const & what)
: runtime_error(what)
{
}
