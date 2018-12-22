#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/Typed.h>
#include <llvmPy/Support/iterator_range.h>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <boost/iterator/indirect_iterator.hpp>
#pragma GCC diagnostic pop

#define DECLARE_AST_MEMBER(T, member, Name) \
public: \
    T const &get##Name() const { return *member; } \
    T &get##Name() { return *member; } \
    void set##Name(T &x) { \
        if (member) { member->removeParent(); } \
        member = &x; \
        member->setParent(this); \
    } \
private: \
    T *member = nullptr;

#define DECLARE_ITER_MEMBER(T, member, name, Name) \
public: \
    using name##_iterator = boost::indirect_iterator<T **>; \
    using name##_const_iterator = boost::indirect_iterator<T const **>; \
    name##_iterator name##_begin() { return boost::make_indirect_iterator(member.data()); } \
    name##_iterator name##_end() { return boost::make_indirect_iterator(member.data() + member.size()); } \
    iterator_range<name##_iterator> name() { return make_range(name##_begin(), name##_end()); } \
    name##_const_iterator name##_begin() const { return boost::make_indirect_iterator(const_cast<T const **>(member.data())); } \
    name##_const_iterator name##_end() const { return boost::make_indirect_iterator(const_cast<T const **>(member.data()) + member.size()); } \
    iterator_range<name##_const_iterator> name() const { return make_range(name##_begin(), name##_end()); } \
    void add##Name (T &it) { \
        member.push_back(&it); \
        it.removeParent(); \
        it.setParent(this); \
    } \
private: \
    std::vector<T *> member;

#ifdef __cplusplus
namespace llvmPy {

using ArgNamesIter = iterator_range<std::string const *>;

class AST : public Typed {
public:
    virtual ~AST();

    virtual void toStream(std::ostream &s) const;

    std::string toString() const;

    bool hasParent() const;

    AST &getParent();

    AST const &getParent() const;

    void setParent(AST &);

    void setParent(AST *);

    void removeParent();

private:
    AST *_parent = nullptr;
};

class Expr : public AST {
public:
    virtual ~Expr();

protected:
    Expr();
};

class StringExpr final : public Expr {
public:
    explicit StringExpr(std::string const &value);

    void toStream(std::ostream &s) const override;

    std::string const &getValue() const;

private:
    std::string const _value;
};

class DecimalExpr final : public Expr {
public:
    explicit DecimalExpr(double value);

    void toStream(std::ostream &s) const override;

    double getValue() const;

private:
    double const _value;
};

class IntegerExpr final : public Expr {
public:
    explicit IntegerExpr(long value);

    void toStream(std::ostream &s) const override;

    int64_t getValue() const;

private:
    long const _value;
};

class IdentExpr final : public Expr {
public:
    explicit IdentExpr(std::string const &name);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

private:
    std::string const _name;
};

class LambdaExpr final : public Expr {
    DECLARE_AST_MEMBER(Expr, _expr, Expr)

public:
    explicit LambdaExpr(Expr &expr);

    void toStream(std::ostream &s) const override;

    std::string const *arg_begin() const;

    std::string const *arg_end() const;

    ArgNamesIter args() const;

    void addArgument(std::string const &argument);

private:
    std::vector<std::string> _args;
};

class UnaryExpr final : public Expr {
    DECLARE_AST_MEMBER(Expr, _expr, Expr)

public:
    UnaryExpr(TokenType op, Expr &expr);

    void toStream(std::ostream &s) const override;

    TokenType getOperator() const;

private:
    TokenType const _operator;
};

class BinaryExpr final : public Expr {
    DECLARE_AST_MEMBER(Expr, _lhs, LeftOperand)
    DECLARE_AST_MEMBER(Expr, _rhs, RightOperand)

public:
    BinaryExpr(Expr &lhs, TokenType op, Expr &rhs);

    void toStream(std::ostream &s) const override;

    TokenType getOperator() const;

private:
    TokenType const _operator;
};

class CallExpr final : public Expr {
    DECLARE_AST_MEMBER(Expr, _callee, Callee)
    DECLARE_ITER_MEMBER(Expr, _args, args, Argument)

public:
    explicit CallExpr(Expr &callee);

    void toStream(std::ostream &s) const override;
};

class TokenExpr final : public Expr {
public:
    explicit TokenExpr(TokenType type);

    void toStream(std::ostream &s) const override;

    TokenType getTokenType() const;

private:
    TokenType _tokenType;
};

/** A group consists of expressions separated by comma (a tuple). */
class TupleExpr final : public Expr {
    DECLARE_ITER_MEMBER(Expr, _members, members, Member)

public:
    using iterator = Expr *;
    using const_iterator = Expr const *;

    explicit TupleExpr();

    void toStream(std::ostream &s) const override;
};

class GetattrExpr final : public Expr {
    DECLARE_AST_MEMBER(Expr, _object, Object)

public:
    GetattrExpr(Expr &object, std::string const &name);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

private:
    std::string const _name;
};

class Stmt : public AST {
public:
    virtual ~Stmt();

protected:
    Stmt();
};

class ExprStmt final : public Stmt {
    DECLARE_AST_MEMBER(Expr, _expr, Expr)

public:
    explicit ExprStmt(Expr &expr);

    void toStream(std::ostream &s) const override;
};

class AssignStmt final : public Stmt {
    DECLARE_AST_MEMBER(Expr, _value, Value)

public:
    AssignStmt(std::string const &name, Expr &value);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

private:
    std::string const _name;
};

class CompoundStmt final : public Stmt {
    DECLARE_ITER_MEMBER(Stmt, _stmts, statements, Statement)

public:
    CompoundStmt();

    explicit CompoundStmt(Stmt &stmt);

    void toStream(std::ostream &s) const override;

    Stmt *findOnlyMember();

    Stmt const *findOnlyMember() const;
};

class DefStmt final : public Stmt {
    DECLARE_AST_MEMBER(CompoundStmt, _body, Body)

public:
    DefStmt(std::string const &name, CompoundStmt &body);

    void toStream(std::ostream &s) const override;

    std::string const &getName() const;

    std::string const *arg_begin() const;

    std::string const *arg_end() const;

    ArgNamesIter args() const;

    void addArgument(std::string const &name);

private:
    std::string const _name;
    std::vector<std::string> _args;
};

class ReturnStmt final : public Stmt {
    DECLARE_AST_MEMBER(Expr, _expr, Expr)

public:
    explicit ReturnStmt(Expr &expr);

    void toStream(std::ostream &s) const override;
};

class PassStmt final : public Stmt {
public:
    PassStmt();

    void toStream(std::ostream &s) const override;
};

class ConditionalStmt final : public Stmt {
    DECLARE_AST_MEMBER(Expr, _condition, Condition)
    DECLARE_AST_MEMBER(CompoundStmt, _thenBranch, ThenBranch)
    DECLARE_AST_MEMBER(CompoundStmt, _elseBranch, ElseBranch)

public:
    ConditionalStmt(
            Expr &condition,
            CompoundStmt &thenBranch,
            CompoundStmt &elseBranch);

    void toStream(std::ostream &s) const override;
};

class WhileStmt final : public Stmt {
    DECLARE_AST_MEMBER(Expr, _condition, Condition)
    DECLARE_AST_MEMBER(CompoundStmt, _body, Body)

public:

    WhileStmt(Expr &condition, CompoundStmt &body);

    void toStream(std::ostream &s) const override;
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
