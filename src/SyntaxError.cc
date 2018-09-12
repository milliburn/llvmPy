#include <llvmPy/SyntaxError.h>
using namespace llvmPy;
using namespace std;

SyntaxError::SyntaxError(std::string const & what)
: runtime_error(what)
{
}
