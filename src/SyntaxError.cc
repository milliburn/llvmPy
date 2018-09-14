#include <llvmPy/SyntaxError.h>
using namespace llvmPy;
using namespace std;

ParserError::ParserError(std::string const & what)
: runtime_error(what)
{
}

SyntaxError::SyntaxError(std::string const & what)
: runtime_error(what)
{
}
