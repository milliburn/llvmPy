#include <llvmPy/Token.h>
#include <assert.h>
using namespace llvmPy;
using namespace std;

Token::Token(TokenType type)
: _type(type), _str(nullptr), _depth(0)
{

}

Token::Token(TokenType type, std::unique_ptr<std::string> str)
: _type(type), _str(std::move(str)), _depth(0)
{

}

Token::Token(TokenType type, size_t depth)
: _type(type), _str(nullptr), _depth(depth)
{
}

TokenType
Token::getTokenType() const
{
    return _type;
}

std::string const &
Token::getString() const
{
    assert(_str);
    return *_str;
}

size_t
Token::getDepth() const
{
    assert(getTokenType() == tok_indent);
    return _depth;
}

std::ostream &
operator<< (std::ostream & s, Token const &t)
{
    switch (t.getTokenType()) {
    case tok_assign: s << '='; break;
    case tok_ident: s << t.getString(); break;
    case tok_indent: s << ">" << t.getDepth(); break;
    case tok_number: s << t.getString() << 'n'; break;
    case tok_string: s << t.getString(); break;
    case tok_eof: break;
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
    case kw_def: s << ">def"; break;
    case kw_lambda: s << ">lambda"; break;
    case kw_import: s << ">import"; break;
    case kw_return: s << ">return"; break;
    case kw_if: s << ">if"; break;
    case kw_elif: s << ">elif"; break;
    case kw_else: s << ">else"; break;
    case kw_pass: s << ">pass"; break;
    case kw_while: s << ">while"; break;
    case kw_break: s << ">break"; break;
    case kw_continue: s << ">continue"; break;
    default: s << "?"; break;
    }

    return s;
}
