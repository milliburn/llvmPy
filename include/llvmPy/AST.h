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

class StringExpr final : public Expr {
public:
    explicit StringExpr(std::string const &value);

    void toStream(std::ostream &s) const override;

    std::string const &getValue() const;

private:
    std::string const value;
};

class DecimalExpr final : public Expr {
public:
    explicit DecimalExpr(double v);

    void toStream(std::ostream &s) const override;

    double getValue() const;

private:
    double const value;
};

class IntegerExpr final : public Expr {
public:
    explicit IntegerExpr(long v);

    void toStream(std::ostream &s) const override;

    int64_t getValue() const;

private:
    long const value;
};

class IdentExpr final : public Expr {
public:
    explicit IdentExpr(std::string const &name);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

private:
    std::string const name;
};

class LambdaExpr final : public Expr {
public:
    explicit LambdaExpr(std::shared_ptr<Expr> const &expr);

    void toStream(std::ostream &s) const override;

    std::vector<std::string const> const &getArguments() const;

    Expr const &getExpr() const;

    std::shared_ptr<Expr const> const &getExprPtr() const;

    void addArgument(std::string const &argument);

private:
    std::vector<std::string const> arguments;

    std::shared_ptr<Expr const> expr;
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
    explicit CallExpr(std::shared_ptr<Expr> const &callee);

    void toStream(std::ostream &s) const override;

public:
    Expr const &getCallee() const;

    std::vector<std::shared_ptr<Expr const>> const &getArguments() const;

    void addArgument(std::shared_ptr<Expr const> argument);

private:
    std::shared_ptr<Expr const> callee;

    std::vector<std::shared_ptr<Expr const>> arguments;
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
    std::vector<std::shared_ptr<Expr const>> const &getMembers() const;

    void addMember(std::shared_ptr<Expr> member);

private:
    std::vector<std::shared_ptr<Expr const>> members;
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
    AssignStmt(
            std::string const &name,
            std::shared_ptr<Expr const> const &value);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

    Expr const &getValue() const;

    std::shared_ptr<Expr const> const &getValuePtr() const;

private:
    std::string const name;
    std::shared_ptr<Expr const> value;
};

class CompoundStmt;

class DefStmt final : public Stmt {
public:
    DefStmt(std::string const &name,
            std::unique_ptr<CompoundStmt> body);

    void toStream(std::ostream &s) const override;

public:
    std::string const &getName() const;

    std::vector<std::string const> const &getArguments() const;

    void addArgument(std::string const &name);

    CompoundStmt const &getBody() const;

private:
    std::string const name;
    std::vector<std::string const> args;
    std::unique_ptr<CompoundStmt> body;
};

class ReturnStmt final : public Stmt {
public:
    explicit ReturnStmt(std::shared_ptr<Expr const> const &expr);

    void toStream(std::ostream &s) const override;

    Expr const &getExpr() const;

    std::shared_ptr<Expr const> const &getExprPtr() const;

private:
    std::shared_ptr<Expr const> expr;
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
