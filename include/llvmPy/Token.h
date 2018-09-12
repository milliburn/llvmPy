#pragma once
#include <string>
#include <vector>
#include <sstream>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum TokenType {
    // Modifiers
    tok_assign = 0x80000000,
    tok_cmpeq = 0x40000000,
    tok_kw = 0x20000000,

    tok_ignore = 1,
    tok_ident,
    tok_indent,
    tok_number,
    tok_string,
    tok_eof,
    tok_eol,

    tok_dot, // .
    tok_semicolon, // ;
    tok_colon, // :
    tok_comma, // ,
    tok_eqsign, // =
    tok_lp, // (
    tok_rp, // )
    tok_lb, // [
    tok_rb, // ]

    // Logical
    tok_not,
    tok_lt,
    tok_gt,

    tok_lte = tok_lt | tok_cmpeq,
    tok_gte = tok_gt | tok_cmpeq,
    tok_neq = tok_not | tok_cmpeq,
    tok_eq = tok_eqsign | tok_cmpeq,

    // Arithmetic
    tok_add, // +
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
};

class Token {
public:
    explicit Token(TokenType type)
        : type(type), str(nullptr) {}

    Token(TokenType type, std::string const * const str)
        : type(type), str(str) {}

    Token(TokenType type, long depth)
        : type(type), str(nullptr) {}

    TokenType type;
    union {
        std::string const * str;
        long depth;
    };
};

} // namespace AST
} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::AST::Token const &);

#endif // __cplusplus
