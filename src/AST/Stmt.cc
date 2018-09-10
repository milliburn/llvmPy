#include <llvmPy/AST.h>
#include <llvmPy/AST/Stmt.h>
#include <vector>
using namespace llvmPy::AST;
using namespace std;

Stmt::Stmt(StmtType type)
: stmtType(type)
{
}

Stmt *
Stmt::parse(std::istream & stream)
{
    bool eol = false;
    StmtType stmtType = StmtType::EXPR;
    vector<Token *> toks;
    Expr * lhs = nullptr;
    Expr * rhs = nullptr;

    while (!eol) {
        Token * tok = Token::parse(stream);
        toks.push_back(tok);

        if (tok->isEOF() || tok->isEOL()) {
            eol = true;
            continue;
        }

        if (stmtType == StmtType::EXPR) {
            switch (tok->tokenType) {
            case TokenType::OPER:
                switch (dynamic_cast<Oper *>(tok)->operType) {
                case OperType::ASSIGN:
                    stmtType = StmtType::ASSIGN;
                    toks.pop_back(); // Remove the assignment operator
                    lhs = Expr::parse(toks);
                    toks.clear();

                    if (lhs->exprType != ExprType::IDENT) {
                        return nullptr;
                    }

                    break;

                default:
                    break;
                }

            default:
                break;
            }
        }
    }

    switch (stmtType) {
    case StmtType::EXPR:
        lhs = Expr::parse(toks);
        return new ExprStmt(*lhs);

    case StmtType::ASSIGN:
        rhs = Expr::parse(toks);
        return new AssignStmt(*dynamic_cast<IdentExpr *>(lhs), *rhs);
    }
}

AssignStmt::AssignStmt(IdentExpr & ident, Expr & expr)
: Stmt(StmtType::ASSIGN), lhs(ident), rhs(expr)
{
}

std::string
AssignStmt::toString() const
{
    return lhs.toString() + "=" + rhs.toString() + "\n";
}

ExprStmt::ExprStmt(Expr & expr)
: Stmt(StmtType::EXPR), expr(expr)
{
}

std::string
ExprStmt::toString() const
{
    return expr.toString();
}
