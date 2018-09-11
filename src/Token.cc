#include <llvmPy/Token.h>
using namespace llvmPy::AST;
using namespace std;

std::ostream &
operator<< (std::ostream & s, Token const &t)
{
    switch (t.type) {
    case tok_assign: s << '='; break;
    case tok_ident: s << t.str; break;
    case tok_indent: s << ">" << t.depth; break;
    case tok_number: s << t.str << 'n'; break;
    case tok_string: s << t.str; break;
    case tok_eof: s << ";eof"; break;
    case tok_eol: s << "\n"; break;
    case tok_dot: s << '.'; break;
    case tok_semicolon: s << ';'; break;
    case tok_colon: s << ':'; break;
    case tok_comma: s << ','; break;
    case tok_eqsign: s << '='; break;
    case tok_lp: s << '('; break;
    case tok_rp: s << ')'; break;
    case tok_lb: s << '['; break;
    case tok_rb: s << ']'; break;
    case tok_not: s << '!'; break;
    case tok_lt: s << '<'; break;
    case tok_gt: s << '>'; break;
    case tok_lte: s << "<="; break;
    case tok_gte: s << ">="; break;
    case tok_neq: s << "!="; break;
    case tok_eq: s << "=="; break;
    case tok_add: s << '+'; break;
    case tok_sub: s << '-'; break;
    case tok_mul: s << '*'; break;
    case tok_div: s << '/'; break;
    case tok_addeq: s << "+="; break;
    case tok_subeq: s << "-="; break;
    case tok_muleq: s << "*="; break;
    case tok_diveq: s << "/="; break;
    default: s << "?"; break;
    }

    return s;
}