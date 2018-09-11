#include <llvmPy/AST.h>
using namespace llvmPy::AST;

std::ostream &
operator<< (std::ostream & s, Expr const & x)
{
    switch (x.type) {
    case expr_ignore: break;
    case expr_ident: s << x.ident; break;
    case expr_strlit: s << '"' << x.strlit << '"'; break;
    case expr_numlit:
        switch (x.numlit.type) {
        case num_double: s << x.numlit.dval << 'd'; break;
        case num_int: s << x.numlit.ival << 'i'; break;
        default: throw "Oops!";
        }
        break;
    default: throw "Oops!";
    }

    return s;
}

std::ostream &
operator<< (std::ostream & s, Stmt const & x)
{
    switch (x.type) {
    case stmt_assign: s << x.assign.lhs << " = " << x.assign.rhs; break;
    case stmt_import: s << "import " << x.import.module; break;
    case stmt_expr: s << x.expr; break;
    default: throw "Oops!";
    }

    return s;
}
