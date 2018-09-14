#pragma once
#include <llvmPy/Token.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class ASTType {
    Ignore,
    Expr,
    ExprIdent,
    ExprStrLit,
    ExprNumLit,
    ExprNumLitDec,
    ExprNumLitInt,
    ExprNumLitAny,
    ExprBinary,
    ExprLambda,
    ExprAny,
    Stmt,
    StmtExpr,
    StmtAssign,
    StmtImport,
    StmtAny,
    Any,
};

class AST {
public:
    ASTType getType() const { return type; }
    virtual void toStream(std::ostream &) const;
    virtual ~AST() = default;

    static bool classof(AST const *) {
        return true;
    }

protected:
    explicit AST(ASTType type) : type(type) {};

public:
    bool isTypeAbove(ASTType t) const {
        return static_cast<int>(type) >= static_cast<int>(t);
    }

    bool isTypeBelow(ASTType t) const {
        return static_cast<int>(type) < static_cast<int>(t);
    }

    bool isTypeBetween(ASTType a, ASTType b) const {
        return isTypeAbove(a) && isTypeBelow(b);
    }

    bool isType(ASTType t) const {
        return type == t;
    }

private:
    ASTType const type;
};

class Expr : public AST {
public:
    static bool classof(AST const *ast) {
        return ast->isTypeBetween(ASTType::Expr, ASTType::ExprAny);
    }

protected:
    explicit Expr(ASTType type) : AST(type) {}
};

class StrLitExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprStrLit);
    }

    std::string const & str;
    explicit StrLitExpr(std::string str)
        : Expr(ASTType::ExprStrLit),
          str(std::move(str)) {}
    void toStream(std::ostream &) const override;
};

class NumLitExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isTypeBetween(ASTType::ExprNumLit, ASTType::ExprNumLitAny);
    }

protected:
    explicit NumLitExpr(ASTType type) : Expr(type) {}
};

class DecLitExpr : public NumLitExpr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprNumLitDec);
    }

    double const value;
    explicit DecLitExpr(double v)
        : NumLitExpr(ASTType::ExprNumLitDec),
          value(v) {}
    void toStream(std::ostream &) const override;
};

class IntLitExpr : public NumLitExpr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprNumLitInt);
    }

    long const value;
    explicit IntLitExpr(long v)
        : NumLitExpr(ASTType::ExprNumLitInt),
          value(v) {}
    void toStream(std::ostream &) const override;
};

class IdentExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprIdent);
    }

    std::string const & name;
    explicit IdentExpr(std::string const * str)
        : Expr(ASTType::ExprIdent),
          name(*str) {}
    void toStream(std::ostream &) const override;
};

class LambdaExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprLambda);
    }

    Expr const & body;
    explicit LambdaExpr(Expr * body)
        : Expr(ASTType::ExprLambda),
          body(*body) {}
    void toStream(std::ostream &) const override;
};

class BinaryExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprBinary);
    }

    Expr const & lhs;
    TokenType const op;
    Expr const & rhs;
    BinaryExpr(Expr * lhs, TokenType op, Expr * rhs)
        : Expr(ASTType::ExprBinary),
          lhs(*lhs),
          op(op),
          rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class Stmt : public AST {
public:
    static bool classof(AST const *ast) {
        return ast->isTypeBetween(ASTType::Stmt, ASTType::StmtAny);
    }

protected:
    explicit Stmt(ASTType type) : AST(type) {}
};

class ExprStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtExpr);
    }

    Expr const & expr;
    explicit ExprStmt(Expr * expr)
        : Stmt(ASTType::StmtExpr),
          expr(*expr) {}
    void toStream(std::ostream &) const override;
};

class AssignStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtAssign);
    }

    std::string const & lhs;
    Expr const & rhs;
    AssignStmt(std::string const * lhs, Expr * rhs)
        : Stmt(ASTType::StmtAssign),
          lhs(*lhs),
          rhs(*rhs) {}
    void toStream(std::ostream &) const override;
};

class ImportStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtImport);
    }

    std::string const & modname;
    explicit ImportStmt(std::string const * modname)
        : Stmt(ASTType::StmtImport),
          modname(*modname) {}
    void toStream(std::ostream &) const override;
};

} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::Stmt const &);

#endif // __cplusplus
