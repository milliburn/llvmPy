#include <llvmPy/AST.h>
#include <llvmPy/AST/Stmt.h>
#include <vector>
using namespace llvmPy::AST;
using namespace std;

Stmt::Stmt(StmtType type)
: stmtType(type)
{
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

PassStmt::PassStmt()
: Stmt(StmtType::PASS)
{

}

std::string
PassStmt::toString() const
{
    return "pass";
}
