#pragma once
#include <llvmPy/AST/Token.h>
#include <iostream>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class Expr {
public:
    static Expr * parse(std::istream&);
    virtual std::string toString();
};

class ConstExpr : public Expr {
public:
    ConstExpr(Token&);
    virtual std::string toString() override;

private:
    Token& t;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr& l, Oper& o, Expr& r);
    virtual std::string toString() override;

private:
    Expr& l;
    Oper& o;
    Expr& r;
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
