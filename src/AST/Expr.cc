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

Expr *
Expr::parse(std::istream& stream)
{
    Token * tok = nullptr;
    Oper * oper = nullptr;
    Syntax * syntax = nullptr;
    Expr * lhs = nullptr;
    Expr * rhs = nullptr;

    //
    // Read LHS
    //

    tok = Token::parse(stream);

    switch (tok->tokenType) {
    case TokenType::IDENT:
        lhs = new IdentExpr(*dynamic_cast<Ident*>(tok));
        tok = Token::parse(stream);
        break;

    case TokenType::LITER:
        lhs = new ConstExpr(*dynamic_cast<Liter*>(tok));
        tok = Token::parse(stream);
        break;

    default:
        return nullptr;
    }

    //
    // Read RHS
    //

    switch (tok->tokenType) {
    case TokenType::SYNTAX:
        syntax = dynamic_cast<Syntax *>(tok);
        switch (syntax->syntaxType) {
        case SyntaxType::END_LINE:
        case SyntaxType::END_FILE:
            return lhs;

        default:
            return nullptr;
        }

    case TokenType::OPER:
        oper = dynamic_cast<Oper *>(tok);
        tok = Token::parse(stream);

        switch (tok->tokenType) {
        case TokenType::IDENT:
            rhs = new IdentExpr(*dynamic_cast<Ident *>(tok));
            break;

        case TokenType::LITER:
            rhs = new ConstExpr(*dynamic_cast<Liter *>(tok));
            break;

        default:
            return nullptr;
        }

        switch (oper->operType) {
        case OperType::ASSIGN:
            if (lhs->exprType != ExprType::IDENT) {
                return nullptr;
            }

            break;

        case OperType::ADD:
        case OperType::SUB:
            // Arithmetic operators
            break;

        default:
            return nullptr;
        }

        return new BinaryExpr(*lhs, *oper, *rhs);

    default:
        return nullptr;
    }
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
