#include <llvmPy/AST.h>
#include <stdexcept>
using namespace llvmPy;

void
AST::toStream(std::ostream &) const
{
    throw std::runtime_error("Not Implemented");
}

void
StrLitExpr::toStream(std::ostream & s) const
{
    s << '"' << str << '"';
}

void
DecLitExpr::toStream(std::ostream & s) const
{
    s << value << 'd';
}

void
IntLitExpr::toStream(std::ostream & s) const
{
    s << value << 'i';
}

void
IdentExpr::toStream(std::ostream & s) const
{
    s << name;
}

void
LambdaExpr::toStream(std::ostream & s) const
{
    s << "(lambda: " << body << ')';
}

void
BinaryExpr::toStream(std::ostream & s) const
{
    s << '(' << lhs << ' ' << Token(op) << ' ' << rhs << ')';
}

void
ExprStmt::toStream(std::ostream & s) const
{
    s << expr;
}

void
AssignStmt::toStream(std::ostream & s) const
{
    s << lhs << " = " << rhs;
}

void
ImportStmt::toStream(std::ostream & s) const
{
    s << modname;
}

std::ostream &
operator<< (std::ostream & s, Expr const & expr)
{
    expr.toStream(s);
    return s;
}

std::ostream &
operator<< (std::ostream & s, Stmt const & stmt)
{
    stmt.toStream(s);
    return s;
}
