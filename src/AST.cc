#include <llvmPy/AST.h>
using namespace llvmPy::AST;

void
Expr::toStream(std::ostream & s) const
{
    throw "Not implemented";
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
    s << "lambda: " << body;
}

void
BinaryExpr::toStream(std::ostream & s) const
{

}

void
Stmt::toStream(std::ostream &) const
{
    throw "Not implemented";
}

void
ExprStmt::toStream(std::ostream & s) const
{
    s << expr;
}

void
AssignStmt::toStream(std::ostream & s) const
{
    s << lhs << '=' << rhs;
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
