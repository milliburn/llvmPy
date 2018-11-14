#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/Typed.h>
#include <string>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

enum class ASTType {
    Ignore,
    Empty,
    Expr,
    ExprToken,
    ExprTuple,
    ExprIdent,
    ExprLit,
    ExprStrLit,
    ExprDecLit,
    ExprIntLit,
    ExprLitAny,
    ExprUnary,
    ExprBinary,
    ExprLambda,
    ExprCall,
    ExprGroup,
    ExprAny,
    Stmt,
    StmtExpr,
    StmtAssign,
    StmtImport,
    StmtDef,
    StmtReturn,
    StmtCompound,
    StmtConditional,
    StmtPass,
    StmtAny,
    Any,
};

class AST : public Typed<ASTType> {
public:
    virtual void toStream(std::ostream &s) const;
    virtual ~AST() = default;

    static bool classof(AST const *) {
        return true;
    }

    std::string toString() const;

protected:
    explicit AST(ASTType type) : Typed(type) {};
};

class EmptyAST : public AST {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::Empty);
    }

    EmptyAST() : AST(ASTType::Empty) {}
    void toStream(std::ostream &s) const override;
};

class Expr : public AST {
public:
    static bool classof(AST const *ast) {
        return ast->isTypeBetween(ASTType::Expr, ASTType::ExprAny);
    }

protected:
    explicit Expr(ASTType type) : AST(type) {}
};

class LitExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isTypeBetween(
                ASTType::ExprLit,
                ASTType::ExprLitAny);
    }

protected:
    explicit LitExpr(ASTType type) : Expr(type) {}
};

class StrLitExpr : public LitExpr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprStrLit);
    }

    explicit StrLitExpr(std::unique_ptr<std::string const> value);
    void toStream(std::ostream &s) const override;
    std::string const &getValue() const;

private:
    std::unique_ptr<std::string const> const value;
};

class DecLitExpr : public LitExpr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprDecLit);
    }

    double const value;
    explicit DecLitExpr(double v)
        : LitExpr(ASTType::ExprDecLit),
          value(v) {}
    void toStream(std::ostream &s) const override;

    double getValue() const { return value; }
};

class IntLitExpr : public LitExpr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprIntLit);
    }

    long const value;
    explicit IntLitExpr(long v)
        : LitExpr(ASTType::ExprIntLit),
          value(v) {}
    void toStream(std::ostream &s) const override;

    int64_t getValue() const { return value; }
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
    void toStream(std::ostream &s) const override;

    std::string const &getName() const { return name; }
};

class LambdaExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprLambda);
    }

    std::vector<std::string const> const args;
    Expr const & expr;

    explicit LambdaExpr(decltype(args) args, Expr * body)
        : Expr(ASTType::ExprLambda),
          args(std::move(args)),
          expr(*body) {}
    void toStream(std::ostream &s) const override;
};

class UnaryExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprUnary);
    }

    UnaryExpr(TokenType op, std::unique_ptr<Expr> expr);
    void toStream(std::ostream &s) const override;
    TokenType getOperator() const;
    Expr const &getExpr() const;

private:
    TokenType const op;
    std::unique_ptr<Expr> const expr;
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
    void toStream(std::ostream &s) const override;
};

class CallExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprCall);
    }

    explicit CallExpr(std::unique_ptr<Expr> callee);
    void toStream(std::ostream &s) const override;

public:
    Expr const &getCallee() const;
    std::vector<std::unique_ptr<Expr const>> const &getArguments() const;
    void addArgument(std::unique_ptr<Expr const> arg);

private:
    std::unique_ptr<Expr const> callee;
    std::vector<std::unique_ptr<Expr const>> arguments;
};

class TokenExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprToken);
    }

    explicit TokenExpr(TokenType type);
    void toStream(std::ostream &s) const override;

public:
    TokenType getTokenType() const;

private:
    TokenType tokenType;
};

/** A group consists of expressions separated by comma (a tuple). */
class TupleExpr : public Expr {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::ExprTuple);
    }

    explicit TupleExpr();
    void toStream(std::ostream &s) const override;

public:
    std::vector<std::unique_ptr<Expr const>> &getMembers();
    void addMember(std::unique_ptr<Expr> member);

private:
    std::vector<std::unique_ptr<Expr const>> members;
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
    void toStream(std::ostream &s) const override;
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
    void toStream(std::ostream &s) const override;
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
    void toStream(std::ostream &s) const override;
};

class CompoundStmt;

class DefStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtDef);
    }

    std::string const &name;
    std::vector<std::string const> args;
    std::unique_ptr<CompoundStmt> body;

    DefStmt(std::string const &name,
            std::vector<std::string const> args,
            std::unique_ptr<CompoundStmt> body)
            : Stmt(ASTType::StmtDef),
              name(name),
              args(std::move(args)),
              body(std::move(body)) {}

    void toStream(std::ostream &s) const override;

public:
    std::string const &getName() const { return name; }
    std::vector<std::string const> const &getArguments() const { return args; }
    CompoundStmt const &getBody() const { return *body; }
};

class ReturnStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtReturn);
    }

    Expr const &expr;
    explicit ReturnStmt(Expr const &expr)
        : Stmt(ASTType::StmtReturn),
          expr(expr) {}
    void toStream(std::ostream &s) const override;
};

class CompoundStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtCompound);
    }

    CompoundStmt();
    void toStream(std::ostream &s) const override;
    std::vector<std::unique_ptr<Stmt const>> const &getStatements() const;
    void addStatement(std::unique_ptr<Stmt> stmt);

private:
    std::vector<std::unique_ptr<Stmt const>> statements;
};

class PassStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtPass);
    }

    PassStmt();

    void toStream(std::ostream &s) const override;
};

class ConditionalStmt : public Stmt {
public:
    static bool classof(AST const *ast) {
        return ast->isType(ASTType::StmtConditional);
    }

    ConditionalStmt(
            std::unique_ptr<Expr> condition,
            std::unique_ptr<Stmt> thenBranch,
            std::unique_ptr<Stmt> elseBranch);

    void toStream(std::ostream &s) const override;

public:
    Expr const &getCondition() const;
    Stmt const &getThenBranch() const;
    Stmt const &getElseBranch() const;

private:
    std::unique_ptr<Expr const> condition;
    std::unique_ptr<Stmt const> thenBranch;
    std::unique_ptr<Stmt const> elseBranch;
};

} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::Stmt const &);

#endif // __cplusplus
