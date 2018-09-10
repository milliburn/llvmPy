#pragma once
#include <llvmPy/AST/Token.h>
#include <llvmPy/Codegen.h>
#include <llvm/IR/Value.h>
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

enum class ExprType {
    IDENT,
    CONST,
    BINARY,
};

class Expr {
public:
    ExprType const exprType;
    static Expr * parse(std::istream&);
    explicit Expr(ExprType);
    virtual llvm::Value * codegen(Codegen&) = 0;
};

class IdentExpr : public Expr {
public:
    Ident & ident;
    explicit IdentExpr(Ident &);
    llvm::Value * codegen(Codegen &) override;
};

class ConstExpr : public Expr {
public:
    Liter& liter;
    explicit ConstExpr(Liter&);
    llvm::Value * codegen(Codegen&) override;
};

class BinaryExpr : public Expr {
public:
    Expr& lhs;
    Oper& op;
    Expr& rhs;
    BinaryExpr(Expr &, Oper&, Expr &);
    llvm::Value * codegen(Codegen&) override;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
