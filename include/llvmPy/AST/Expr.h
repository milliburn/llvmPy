#pragma once
#include <llvmPy/AST/Token.h>
#include <llvmPy/Codegen.h>
#include <llvm/IR/Value.h>
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class Expr {
public:
    static Expr * parse(std::istream&);
    virtual std::string toString();
    virtual llvm::Value * codegen(Codegen&) = 0;
};

class ConstExpr : public Expr {
public:
    ConstExpr(Token&);
    std::string toString() override;
    llvm::Value * codegen(Codegen&) override;

private:
    Token& t;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr& l, Oper& o, Expr& r);
    std::string toString() override;
    llvm::Value * codegen(Codegen&) override;

private:
    Expr& l;
    Oper& o;
    Expr& r;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
