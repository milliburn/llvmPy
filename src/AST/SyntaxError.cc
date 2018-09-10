#include <llvmPy/AST/SyntaxError.h>
using namespace llvmPy::AST;
using namespace std;

SyntaxError::SyntaxError(std::string const & what)
: runtime_error(what)
{
}
