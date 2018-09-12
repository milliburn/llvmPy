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
    virtual ~Expr() = default;
    virtual void toStream(std::ostream &) const;
protected:
    Expr() = default;
};

class StrLitExpr : public Expr {
public:
    std::string const & str;
    StrLitExpr(std::string str) : str(std::move(str)) {}
    void toStream(std::ostream &) const override;
};

class NumLitExpr : public Expr {
protected:
    NumLitExpr() = default;
};

class DecLitExpr : public NumLitExpr {
public:
    double const value;
    explicit DecLitExpr(double v) : value(v) {}
    void toStream(std::ostream &) const override;
};

class IntLitExpr : public NumLitExpr {
public:
    long const value;
    explicit IntLitExpr(long v) : value(v) {}
    void toStream(std::ostream &) const override;
};

class IdentExpr : public Expr {
public:
    std::string const & name;
    explicit IdentExpr(std::string const * str) : name(*str) {}
    void toStream(std::ostream &) const override;
};

class LambdaExpr : public Expr {
public:
    Expr const & body;
    explicit LambdaExpr(Expr * body) : body(*body) {}
    void toStream(std::ostream &) const override;
};

class BinaryExpr : public Expr {
public:
    Expr const & lhs;
    TokenType const op;
    Expr const & rhs;
    BinaryExpr(Expr * lhs, TokenType op, Expr * rhs)
        : lhs(*lhs),
          op(op),
          rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void toStream(std::ostream &) const;

protected:
    Stmt() = default;
};

class ExprStmt : public Stmt {
public:
    Expr const & expr;
    explicit ExprStmt(Expr * expr) : expr(*expr) {}
    void toStream(std::ostream &) const override;
};

class AssignStmt : public Stmt {
public:
    std::string const & lhs;
    Expr const & rhs;
    AssignStmt(std::string const * lhs, Expr * rhs) : lhs(*lhs), rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class ImportStmt : public Stmt {
public:
    std::string const & modname;
    explicit ImportStmt(std::string const * modname) : modname(*modname) {}
    void toStream(std::ostream &) const override;
};

} // namespace AST
} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::AST::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::AST::Stmt const &);

#endif // __cplusplus
