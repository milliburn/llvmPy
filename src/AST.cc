#include <llvmPy/AST.h>
#include <stdexcept>
using namespace llvmPy;
using std::ostream;
using std::string;
using std::stringstream;
using std::endl;

static constexpr int INDENT = 4;

static void
indentToStream(ostream &s, Stmt const &stmt, int indent)
{
    std::stringstream ss;
    ss << stmt;
    ss.seekg(0, std::ios::beg);
    std::string indents(indent, ' ');

    std::string _s = ss.str();

    for (;;) {
        std::string line;
        std::getline(ss, line);

        if (ss.fail() || ss.eof()) {
            break;
        }

        s << indents;
        s << line;
        s << std::endl; // getline() discards the delimiter.
    }
}

std::string
AST::toString() const
{
    std::ostringstream ss;
    toStream(ss);
    return ss.str();
}

void
AST::toStream(std::ostream &s) const
{
    throw std::runtime_error("Not Implemented");
}

void
EmptyAST::toStream(std::ostream &s) const
{
}

void
StrLitExpr::toStream(std::ostream &s) const
{
    s << '"' << getValue() << '"';
}

void
DecLitExpr::toStream(std::ostream &s) const
{
    s << value << 'd';
}

void
IntLitExpr::toStream(std::ostream &s) const
{
    s << value << 'i';
}

void
IdentExpr::toStream(std::ostream &s) const
{
    s << name;
}

void
LambdaExpr::toStream(std::ostream &s) const
{
    s << "(lambda";

    for (int i = 0; i < args.size(); ++i) {
        if (i > 0) s << ',';
        s << ' ' << args[i];
    }

    s << ": " << expr << ')';
}

void
BinaryExpr::toStream(std::ostream &s) const
{
    s << '(' << lhs << ' ' << Token(op) << ' ' << rhs << ')';
}

void
CallExpr::toStream(std::ostream &s) const
{
    s << getCallee() << '(';

    auto &args = getArguments();

    for (int i = 0; i < args.size(); ++i) {
        if (i > 0) s << ", ";
        s << *args[i];
    }

    s << ')';
}

void
ExprStmt::toStream(std::ostream &s) const
{
    s << expr << endl;
}

void
AssignStmt::toStream(std::ostream &s) const
{
    s << lhs << " = " << rhs << endl;
}

void
ImportStmt::toStream(std::ostream &s) const
{
    s << modname;
}

void
DefStmt::toStream(std::ostream &s) const
{
    s << "def " << name << "(";

    for (int i = 0; i < args.size(); ++i) {
        if (i > 0) s << ", ";
        s << args[i];
    }

    s << "):" << endl;

    for (auto const &stmt : body->getStatements()) {
        indentToStream(s, *stmt, INDENT);
    }
}

void
ReturnStmt::toStream(std::ostream &s) const
{
    s << "return " << expr << endl;
}

std::ostream &
operator<< (std::ostream & s, Expr const & expr)
{
    expr.toStream(s);
    return s;
}

std::ostream &
operator<< (std::ostream & s, Stmt const & stmt)
{
    stmt.toStream(s);
    return s;
}

StrLitExpr::StrLitExpr(std::unique_ptr<std::string const> value)
: LitExpr(ASTType::ExprStrLit),
  value(std::move(value))
{
}

std::string const &
StrLitExpr::getValue() const
{
    return *value;
}

CallExpr::CallExpr(std::unique_ptr<Expr> callee)
: Expr(ASTType::ExprCall),
  callee(std::move(callee))
{
}

Expr const &
CallExpr::getCallee() const
{
    return *callee;
}

std::vector<std::unique_ptr<Expr const>> const &
CallExpr::getArguments() const
{
    return arguments;
}

void
CallExpr::addArgument(std::unique_ptr<Expr const> arg)
{
    arguments.push_back(std::move(arg));
}

TupleExpr::TupleExpr()
: Expr(ASTType::ExprTuple)
{

}

void
TupleExpr::toStream(std::ostream &s) const
{
    s << "(";

    int i = 0;
    for (auto const &member : members) {
        if (i > 0) {
            s << ", ";
        }

        s << *member;
        i += 1;
    }

    if (members.size() == 1) {
        s << ",";
    }

    s << ")";
}

std::vector<std::unique_ptr<Expr const>> &
TupleExpr::getMembers()
{
    return members;
}

void
TupleExpr::addMember(std::unique_ptr<Expr> member)
{
    members.push_back(std::move(member));
    // members.insert(members.begin(), std::move(member));
}

TokenExpr::TokenExpr(TokenType type)
: Expr(ASTType::ExprToken), tokenType(type)
{

}

void
TokenExpr::toStream(std::ostream &s) const
{
    AST::toStream(s);
}

TokenType
TokenExpr::getTokenType() const
{
    return tokenType;
}

UnaryExpr::UnaryExpr(TokenType op, std::unique_ptr<Expr> expr)
: Expr(ASTType::ExprUnary), op(op), expr(std::move(expr))
{
}

void
UnaryExpr::toStream(std::ostream &s) const
{
    Token t(getOperator());
    s << t;
    s << getExpr();
}

TokenType
UnaryExpr::getOperator() const
{
    return op;
}

Expr const &
UnaryExpr::getExpr() const
{
    return *expr;
}

CompoundStmt::CompoundStmt()
: Stmt(ASTType::StmtCompound)
{
}

void
CompoundStmt::toStream(std::ostream &s) const
{
    for (auto const &stmt : getStatements()) {
        s << *stmt;
    }
}

std::vector<std::unique_ptr<Stmt const>> const &
CompoundStmt::getStatements() const
{
    return statements;
}

void
CompoundStmt::addStatement(std::unique_ptr<Stmt> stmt)
{
    statements.push_back(std::move(stmt));
}

PassStmt::PassStmt()
: Stmt(ASTType::StmtPass)
{
}

void
PassStmt::toStream(std::ostream &s) const
{
    s << "pass" << endl;
}

ConditionalStmt::ConditionalStmt(
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Stmt> thenBranch,
        std::unique_ptr<Stmt> elseBranch)
: Stmt(ASTType::StmtConditional),
  condition(std::move(condition)),
  thenBranch(std::move(thenBranch)),
  elseBranch(std::move(elseBranch))
{
    assert(condition && thenBranch && elseBranch);
}

void
ConditionalStmt::toStream(std::ostream &s) const
{
    // TODO: Pretty-printing elifs.
    s << "if " << *condition << ":" << endl;
    indentToStream(s, *thenBranch, INDENT);
    s << "else:" << endl;
    indentToStream(s, *elseBranch, INDENT);
}

Expr const &
ConditionalStmt::getCondition() const
{
    return *condition;
}

Stmt const &
ConditionalStmt::getThenBranch() const
{
    return *thenBranch;
}

Stmt const &
ConditionalStmt::getElseBranch() const
{
    return *elseBranch;
}
