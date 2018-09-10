#include <llvmPy/AST/Expr.h>
#include <llvmPy/AST/Token.h>
#include <typeinfo>
using namespace llvmPy::AST;

Expr *
Expr::parse(std::istream& stream)
{
    Token * tok = nullptr;
    Oper * oper = nullptr;
    Expr * lhs = nullptr;
    Expr * rhs = nullptr;

    tok = Token::parse(stream);

    //
    // Read LHS
    //

    switch (tok->getType()) {
    case TokenType::EOL:
        return nullptr;

    case TokenType::LPARENS:
        lhs = Expr::parse(stream);
        tok = Token::parse(stream);
        if (tok->getType() != TokenType::RPARENS) {
            return nullptr;
        }
        break;

    case TokenType::NUM_LIT:
    case TokenType::STR_LIT:
        lhs = new ConstExpr(*tok);
        tok = Token::parse(stream);
        break;
    }

    //
    // Read RHS
    //

    switch (tok->getType()) {
    case TokenType::EOL:
        return lhs;

    case TokenType::OPER:
        oper = (Oper *) tok;
        tok = Token::parse(stream);

        switch (tok->getType()) {
        case TokenType::NUM_LIT:
        case TokenType::STR_LIT:
            rhs = new ConstExpr(*tok);
            break;

        default:
            return nullptr;
        }

        return new BinaryExpr(*lhs, *oper, *rhs);

    default:
        return nullptr;
    }
}

std::string
Expr::toString()
{
    return "EXPR";
}

ConstExpr::ConstExpr(Token& t)
: t(t)
{
}

std::string
ConstExpr::toString()
{
    return t.toString();
}

BinaryExpr::BinaryExpr(Expr & l, Oper& o, Expr & r)
: l(l), o(o), r(r)
{
}

std::string
BinaryExpr::toString()
{
    return "(" + l.toString() + " " + o.toString() + " " + r.toString() + ")";
}
