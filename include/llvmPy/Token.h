#pragma once
#include <string>
#include <vector>
#include <sstream>

#ifdef __cplusplus
namespace llvmPy {

enum TokenType {
    tok_eof = 0,
    tok_unknown = tok_eof,

    // Modifiers
    tok_assign = 0x80000000,
    tok_cmpeq = 0x40000000,
    tok_kw = 0x20000000,
    tok_oper = 0x10000000,

    tok_ignore = 1,
    tok_ident,
    tok_indent,
    tok_number,
    tok_string,
    tok_eol,

    tok_semicolon, // ;
    tok_colon, // :
    tok_comma, // ,
    tok_eqsign, // =
    tok_not,
    tok_lp, // (
    tok_rp, // )
    tok_lb, // [
    tok_rb, // ]

    // Operators
    tok_dot = 100 | tok_oper, // .

    // Logical operators
    tok_lt,
    tok_gt,

    tok_lte = tok_lt | tok_cmpeq | tok_oper,
    tok_gte = tok_gt | tok_cmpeq | tok_oper,
    tok_neq = tok_not | tok_cmpeq | tok_oper,
    tok_eq = tok_eqsign | tok_cmpeq | tok_oper,

    // Arithmetic operators
    tok_add = 200 | tok_oper, // +
    tok_sub, // -
    tok_mul, // *
    tok_div, // /

    tok_addeq = tok_add | tok_assign,
    tok_subeq = tok_sub | tok_assign,
    tok_muleq = tok_mul | tok_assign,
    tok_diveq = tok_div | tok_assign,

    // Keywords
    kw_def = 1000 | tok_kw,
    kw_lambda,
    kw_import,
    kw_return,
    kw_if,
    kw_elif,
    kw_else,
    kw_pass,
    kw_while,
};

class Token {
public:
    explicit Token(TokenType type)
        : type(type), str(nullptr) {}

    Token(TokenType type, std::string const * const str)
        : type(type), str(str) {}

    Token(TokenType type, long depth)
        : type(type), depth(depth) {}

    TokenType type;
    union {
        std::string const * str;
        long depth;
    };
};

} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::Token const &);

#endif // __cplusplus
