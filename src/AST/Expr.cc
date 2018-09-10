#include <algorithm>
#include <llvmPy/AST/Expr.h>
#include <llvmPy/AST/Token.h>
#include <typeinfo>
using namespace llvm;
using namespace llvmPy::AST;
using std::move;

Expr::Expr(ExprType exprType)
: exprType(exprType)
{
}

IdentExpr::IdentExpr(Ident & ident)
: Expr(ExprType::IDENT), ident(ident)
{
}

llvm::Value *
IdentExpr::codegen(llvmPy::Codegen &)
{
    return nullptr;
}

std::string
IdentExpr::toString() const
{
    return ident.toString();
}

ConstExpr::ConstExpr(Liter& liter)
: Expr(ExprType::CONST), liter(liter)
{
}

llvm::Value *
ConstExpr::codegen(Codegen& cg)
{
    switch (liter.literType) {
    case LiterType::BOOL:
        return ConstantInt::get(
                cg.getContext(),
                APInt(64, (uint64_t) (liter.bval ? 1 : 0), true));

    case LiterType::DEC:
        return ConstantFP::get(
                cg.getContext(),
                APFloat(liter.dval));

    case LiterType::INT:
        return ConstantInt::get(
                cg.getContext(),
                APInt(64, (uint64_t) liter.ival, true));

    default:
        return nullptr;
    }
}

std::string
ConstExpr::toString() const
{
    return liter.toString();
}

BinaryExpr::BinaryExpr(
        Expr& lhs,
        Oper& op,
        Expr& rhs)
: Expr(ExprType::BINARY), lhs(lhs), op(op), rhs(rhs)
{
}

llvm::Value *
BinaryExpr::codegen(Codegen& cg)
{
    Value * L = lhs.codegen(cg);
    Value * R = rhs.codegen(cg);

    if (!L || !R)
        return nullptr;

    switch (op.operType) {
    case OperType::ADD:
        return cg.getBuilder().CreateFAdd(L, R);

    case OperType::SUB:
        return cg.getBuilder().CreateFSub(L, R);

    default:
        return nullptr;
    }
}

std::string
BinaryExpr::toString() const
{
    return "(" + lhs.toString() + op.toString() + rhs.toString() + ")";
}

std::ostream& operator<< (std::ostream & os, Expr const & value) {
    os << value.toString();
    return os;
}
