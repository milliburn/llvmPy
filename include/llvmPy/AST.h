#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/Typed.h>
#include <string>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class AST : public Typed {
public:
    virtual ~AST() = default;

    virtual void toStream(std::ostream &s) const;

    std::string toString() const;
};

class Expr : public AST {
protected:
    Expr();
};

class StrLitExpr final : public Expr {
public:
    explicit StrLitExpr(std::unique_ptr<std::string const> value);

    void toStream(std::ostream &s) const override;

    std::string const &getValue() const;

private:
    std::unique_ptr<std::string const> const value;
};

class DecLitExpr final : public Expr {
public:
    explicit DecLitExpr(double v);

    void toStream(std::ostream &s) const override;

    double getValue() const;

private:
    double const value;
};

class IntLitExpr final : public Expr {
public:
    explicit IntLitExpr(long v);

    void toStream(std::ostream &s) const override;

    int64_t getValue() const;

private:
    long const value;
};

class IdentExpr final : public Expr {
public:
    std::string const & name;

    explicit IdentExpr(std::string const * str);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;
};

class LambdaExpr final : public Expr {
public:
    std::vector<std::string const> const args;

    Expr const & expr;

    explicit
    LambdaExpr(
            std::vector<std::string const> const args,
            Expr * body);

    void toStream(std::ostream &s) const override;
};

class UnaryExpr final : public Expr {
public:
    UnaryExpr(TokenType op, std::unique_ptr<Expr> expr);

    void toStream(std::ostream &s) const override;

    TokenType getOperator() const;

    Expr const &getExpr() const;

private:
    TokenType const op;
    std::unique_ptr<Expr> const expr;
};

class BinaryExpr final : public Expr {
public:
    Expr const & lhs;
    TokenType const op;
    Expr const & rhs;

    BinaryExpr(Expr * lhs, TokenType op, Expr * rhs);

    void toStream(std::ostream &s) const override;
};

class CallExpr final : public Expr {
public:
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

class TokenExpr final : public Expr {
public:
    explicit TokenExpr(TokenType type);

    void toStream(std::ostream &s) const override;

    TokenType getTokenType() const;

private:
    TokenType tokenType;
};

/** A group consists of expressions separated by comma (a tuple). */
class TupleExpr final : public Expr {
public:
    explicit TupleExpr();

    void toStream(std::ostream &s) const override;

public:
    std::vector<std::unique_ptr<Expr const>> &getMembers();

    void addMember(std::unique_ptr<Expr> member);

private:
    std::vector<std::unique_ptr<Expr const>> members;
};

class Stmt : public AST {
protected:
    Stmt();
};

class ExprStmt final : public Stmt {
public:
    Expr const & expr;

    explicit ExprStmt(Expr * expr);

    void toStream(std::ostream &s) const override;
};

class AssignStmt final : public Stmt {
public:
    std::string const & lhs;

    Expr const & rhs;

    AssignStmt(std::string const * lhs, Expr * rhs);

    void toStream(std::ostream &s) const override;
};

class CompoundStmt;

class DefStmt final : public Stmt {
public:
    std::string const &name;
    std::vector<std::string const> args;
    std::unique_ptr<CompoundStmt> body;

    DefStmt(std::string const &name,
            std::vector<std::string const> args,
            std::unique_ptr<CompoundStmt> body);

    void toStream(std::ostream &s) const override;

public:
    std::string const &getName() const { return name; }
    std::vector<std::string const> const &getArguments() const { return args; }
    CompoundStmt const &getBody() const { return *body; }
};

class ReturnStmt final : public Stmt {
public:
    Expr const &expr;

    explicit ReturnStmt(Expr const &expr);

    void toStream(std::ostream &s) const override;
};

class CompoundStmt final : public Stmt {
public:
    CompoundStmt();
    void toStream(std::ostream &s) const override;
    std::vector<std::unique_ptr<Stmt const>> const &getStatements() const;
    void addStatement(std::unique_ptr<Stmt> stmt);

private:
    std::vector<std::unique_ptr<Stmt const>> statements;
};

class PassStmt final : public Stmt {
public:
    PassStmt();

    void toStream(std::ostream &s) const override;
};

class ConditionalStmt final : public Stmt {
public:
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

class WhileStmt final : public Stmt {
public:

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body);

    void toStream(std::ostream &s) const override;

    Expr const &getCondition() const;

    Stmt const &getBody() const;

private:
    std::unique_ptr<Expr const> condition;
    std::unique_ptr<Stmt const> body;
};

class BreakStmt final : public Stmt {
public:
    void toStream(std::ostream &s) const override;
};

class ContinueStmt final : public Stmt {
public:
    void toStream(std::ostream &s) const override;
};

} // namespace llvmPy

std::ostream & operator<< (std::ostream &, llvmPy::Expr const &);
std::ostream & operator<< (std::ostream &, llvmPy::Stmt const &);

#endif // __cplusplus
