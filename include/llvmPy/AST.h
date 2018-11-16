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
    UnaryExpr(TokenType op, std::shared_ptr<Expr const> const &expr);

    void toStream(std::ostream &s) const override;

    TokenType getOperator() const;

    Expr const &getExpr() const;

private:
    TokenType const op;
    std::shared_ptr<Expr const> expr;
};

class BinaryExpr final : public Expr {
public:
    BinaryExpr(
            std::shared_ptr<Expr const> const &lhs,
            TokenType op,
            std::shared_ptr<Expr const> const &rhs);

    void toStream(std::ostream &s) const override;

    Expr const &getLeftOperand() const;

    Expr const &getRightOperand() const;

    TokenType getOperator() const;

private:
    std::shared_ptr<Expr const> lhs;
    std::shared_ptr<Expr const> rhs;
    TokenType const op;
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
    explicit ExprStmt(std::shared_ptr<Expr const> const &expr);

    void toStream(std::ostream &s) const override;

    Expr const &getExpr() const;

private:
    std::shared_ptr<Expr const> expr;
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

class DefStmt final : public Stmt {
public:
    DefStmt(std::string const &name,
            std::shared_ptr<Stmt const> const &body);

    void toStream(std::ostream &s) const override;

public:
    std::string const &getName() const;

    std::vector<std::string const> const &getArguments() const;

    void addArgument(std::string const &name);

    Stmt const &getBody() const;

private:
    std::string const name;
    std::vector<std::string const> args;
    std::shared_ptr<Stmt const> body;
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

    std::vector<std::shared_ptr<Stmt const>> const &getStatements() const;

    void addStatement(std::shared_ptr<Stmt const> const &stmt);

private:
    std::vector<std::shared_ptr<Stmt const>> statements;
};

class PassStmt final : public Stmt {
public:
    PassStmt();

    void toStream(std::ostream &s) const override;
};

class ConditionalStmt final : public Stmt {
public:
    ConditionalStmt(
            std::shared_ptr<Expr const> const &condition,
            std::shared_ptr<Stmt const> const &thenBranch,
            std::shared_ptr<Stmt const> const &elseBranch);

    void toStream(std::ostream &s) const override;

public:
    Expr const &getCondition() const;

    Stmt const &getThenBranch() const;

    Stmt const &getElseBranch() const;

private:
    std::shared_ptr<Expr const> condition;
    std::shared_ptr<Stmt const> thenBranch;
    std::shared_ptr<Stmt const> elseBranch;
};

class WhileStmt final : public Stmt {
public:

    WhileStmt(
            std::shared_ptr<Expr const> const &condition,
            std::shared_ptr<Stmt const> const &body);

    void toStream(std::ostream &s) const override;

    Expr const &getCondition() const;

    Stmt const &getBody() const;

private:
    std::shared_ptr<Expr const> condition;
    std::shared_ptr<Stmt const> body;
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
