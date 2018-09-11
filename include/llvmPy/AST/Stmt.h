#pragma once
#include <llvmPy/Token.h>
#include <llvmPy/AST/Expr.h>
#include <llvmPy/Codegen.h>
#include <llvm/IR/Value.h>
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum class StmtType {
    ASSIGN,
    EXPR,
    PASS,
};

class Stmt {
public:
    StmtType const stmtType;
    virtual std::string toString() const = 0;

protected:
    explicit Stmt(StmtType);
};

class AssignStmt : public Stmt {
public:
    IdentExpr & lhs;
    Expr & rhs;
    AssignStmt(IdentExpr&, Expr&);
    std::string toString() const override;
};

class ExprStmt : public Stmt {
public:
    Expr & expr;
    explicit ExprStmt(Expr &);
    std::string toString() const override;
};

class PassStmt : public Stmt {
public:
    explicit PassStmt();
    std::string toString() const override;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus