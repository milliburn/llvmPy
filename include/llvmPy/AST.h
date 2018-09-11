#pragma once
#include <llvmPy/Token.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum ExprType {
    expr_ignore,
    expr_ident,
    expr_strlit,
    expr_numlit,
    expr_boollit,
};

enum NumType {
    num_double,
    num_int,
};

enum StmtType {
    stmt_ignore,
    stmt_expr,
    stmt_assign,
    stmt_import,
};

class Expr {
public:
    Expr() : type(expr_ignore) {}

    ExprType type;
    Token ident;
    std::string strlit;

    union {
        bool bval;

        struct {
            NumType type = num_int;
            union {
                double dval;
                long ival = 0;
            };
        } numlit;

        struct {
            Expr * lhs = nullptr;
            Expr * rhs;
        } cmp;
    };
};

class Stmt {
public:
    Stmt() : type(stmt_ignore) {}

    StmtType type;
    Expr expr;

    struct {
        Token lhs;
        Expr rhs;
    } assign;

    struct {
        Token module;
    } import;
};

} // namespace AST
} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::AST::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::AST::Stmt const &);

#endif // __cplusplus
