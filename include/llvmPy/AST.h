#pragma once
#include <llvmPy/Token.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

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

class AST {
public:
    enum ASTType {
        ast_ignore,
        expr,
        expr_ident,
        expr_strlit,
        expr_numlit,
        expr_numlit_dec,
        expr_numlit_int,
        expr_numlit_any,
        expr_binary,
        expr_lambda,
        stmt,
        stmt_expr,
        stmt_assign,
        stmt_import,
        stmt_any,
    };

    ASTType getType() const { return type; }
    virtual void toStream(std::ostream &) const;
    virtual ~AST() = default;

protected:
    explicit AST(ASTType type) : type(type) {};

private:
    ASTType const type;
};

class Expr : public AST {
protected:
    explicit Expr(ASTType type) : AST(type) {}
};

class StrLitExpr : public Expr {
public:
    std::string const & str;
    explicit StrLitExpr(std::string str)
        : Expr(expr_strlit),
          str(std::move(str)) {}
    void toStream(std::ostream &) const override;
};

class NumLitExpr : public Expr {
protected:
    explicit NumLitExpr(ASTType type) : Expr(type) {}
};

class DecLitExpr : public NumLitExpr {
public:
    double const value;
    explicit DecLitExpr(double v) : NumLitExpr(expr_numlit_dec), value(v) {}
    void toStream(std::ostream &) const override;
};

class IntLitExpr : public NumLitExpr {
public:
    long const value;
    explicit IntLitExpr(long v) : NumLitExpr(expr_numlit_int), value(v) {}
    void toStream(std::ostream &) const override;
};

class IdentExpr : public Expr {
public:
    std::string const & name;
    explicit IdentExpr(std::string const * str)
        : Expr(expr_ident), name(*str) {}
    void toStream(std::ostream &) const override;
};

class LambdaExpr : public Expr {
public:
    Expr const & body;
    explicit LambdaExpr(Expr * body)
        : Expr(expr_lambda), body(*body) {}
    void toStream(std::ostream &) const override;
};

class BinaryExpr : public Expr {
public:
    Expr const & lhs;
    TokenType const op;
    Expr const & rhs;
    BinaryExpr(Expr * lhs, TokenType op, Expr * rhs)
        : Expr(expr_binary),
          lhs(*lhs),
          op(op),
          rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class Stmt : public AST {
protected:
    explicit Stmt(ASTType type) : AST(type) {}
};

class ExprStmt : public Stmt {
public:
    Expr const & expr;
    explicit ExprStmt(Expr * expr) : Stmt(stmt_expr), expr(*expr) {}
    void toStream(std::ostream &) const override;
};

class AssignStmt : public Stmt {
public:
    std::string const & lhs;
    Expr const & rhs;
    AssignStmt(std::string const * lhs, Expr * rhs)
        : Stmt(stmt_assign),
          lhs(*lhs),
          rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class ImportStmt : public Stmt {
public:
    std::string const & modname;
    explicit ImportStmt(std::string const * modname)
        : Stmt(stmt_import),
          modname(*modname) {}
    void toStream(std::ostream &) const override;
};

} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::Stmt const &);

#endif // __cplusplus
