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
StringExpr::toStream(std::ostream &s) const
{
    s << '"' << getValue() << '"';
}

DecimalExpr::DecimalExpr(double v)
: value(v)
{
}

double
DecimalExpr::getValue() const
{
    return value;
}

void
DecimalExpr::toStream(std::ostream &s) const
{
    s << value << 'd';
}

IntegerExpr::IntegerExpr(long v)
: value(v)
{
}

int64_t
IntegerExpr::getValue() const
{
    return value;
}

void
IntegerExpr::toStream(std::ostream &s) const
{
    s << value << 'i';
}

IdentExpr::IdentExpr(std::unique_ptr<std::string const> name)
: name(std::move(name))
{
}

std::string const &
IdentExpr::getName() const
{
    assert(name);
    return *name;
}

void
IdentExpr::toStream(std::ostream &s) const
{
    s << getName();
}

LambdaExpr::LambdaExpr(std::shared_ptr<Expr> expr)
: expr(expr)
{
}

std::vector<std::shared_ptr<std::string const>> const &
LambdaExpr::getArguments() const
{
    return arguments;
}

void
LambdaExpr::addArgument(std::shared_ptr<std::string const> name)
{
    arguments.emplace_back(name);
}

Expr const &
LambdaExpr::getExpr() const
{
    return *expr;
}

void
LambdaExpr::toStream(std::ostream &s) const
{
    s << "(lambda";

    for (int i = 0; i < arguments.size(); ++i) {
        if (i > 0) s << ',';
        s << ' ' << *arguments[i];
    }

    s << ": " << getExpr() << ')';
}

BinaryExpr::BinaryExpr(Expr *lhs, TokenType op, Expr *rhs)
: lhs(*lhs), op(op), rhs(*rhs)
{
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

    int iArg = 0;
    for (auto const &arg : getArguments()) {
        if (iArg > 0) {
            s << ", ";
        }

        s << *arg;
        iArg += 1;
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
    s << *name << " = " << *value << endl;
}

void
DefStmt::toStream(std::ostream &s) const
{
    s << "def " << getName() << "(";

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
    s << "return " << *expr << endl;
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

StringExpr::StringExpr(std::unique_ptr<std::string const> value)
: value(std::move(value))
{
}

std::string const &
StringExpr::getValue() const
{
    return *value;
}

CallExpr::CallExpr(std::shared_ptr<Expr> const &callee)
: callee(callee)
{
}

Expr const &
CallExpr::getCallee() const
{
    return *callee;
}

std::vector<std::shared_ptr<Expr const>> const &
CallExpr::getArguments() const
{
    return arguments;
}

void
CallExpr::addArgument(std::shared_ptr<Expr const> argument)
{
    arguments.emplace_back(argument);
}

TupleExpr::TupleExpr()
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

std::vector<std::shared_ptr<Expr const>> const &
TupleExpr::getMembers() const
{
    return members;
}

void
TupleExpr::addMember(std::shared_ptr<Expr> member)
{
    members.emplace_back(member);
}

TokenExpr::TokenExpr(TokenType type)
: tokenType(type)
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
: op(op), expr(std::move(expr))
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
: condition(std::move(condition)),
  thenBranch(std::move(thenBranch)),
  elseBranch(std::move(elseBranch))
{
    assert(this->condition);
    assert(this->thenBranch);
    assert(this->elseBranch);
}

void
ConditionalStmt::toStream(std::ostream &s) const
{
    s << "if " << *condition << ":" << endl;
    indentToStream(s, *thenBranch, INDENT);

    if (elseBranch->isa<ConditionalStmt>()) {
        // XXX: This relies on toStream() not prepending anything, i.e. we get
        // XXX: the "el" from here and the "if" from the next toStream().
        s << "el";
        s << *elseBranch;
    } else if (!elseBranch->isa<PassStmt>()) {
        s << "else:" << endl;
        indentToStream(s, *elseBranch, INDENT);
    }
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

ExprStmt::ExprStmt(Expr *expr)
: expr(*expr)
{
}

DefStmt::DefStmt(
        std::unique_ptr<std::string const> name,
        std::vector<std::string const> args,
        std::unique_ptr<CompoundStmt> body)
: name(std::move(name)),
  args(std::move(args)),
  body(std::move(body))
{
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr const> expr)
: expr(std::move(expr))
{
}

Expr const &
ReturnStmt::getExpr() const
{
    return *expr;
}

std::unique_ptr<Expr const>
ReturnStmt::releaseExpr()
{
    return std::move(expr);
}

Expr::Expr() = default;

Stmt::Stmt() = default;

WhileStmt::WhileStmt(
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Stmt> body)
: condition(std::move(condition)),
  body(std::move(body))
{
}

void
WhileStmt::toStream(std::ostream &s) const
{
    s << "while " << *condition << ":" << endl;
    indentToStream(s, *body, INDENT);
}

Expr const &
WhileStmt::getCondition() const
{
    return *condition;
}

Stmt const &
WhileStmt::getBody() const
{
    return *body;
}

void
BreakStmt::toStream(std::ostream &s) const
{
    s << "break" << endl;
}

void
ContinueStmt::toStream(std::ostream &s) const
{
    s << "continue" << endl;
}

AssignStmt::AssignStmt(
        std::unique_ptr<std::string const> name,
        std::unique_ptr<Expr const> value)
: name(std::move(name)),
  value(std::move(value)),
  lhs(getName()),
  rhs(getValue())
{
}

std::string const &
AssignStmt::getName() const
{
    return *name;
}

Expr const &
AssignStmt::getValue() const
{
    return *value;
}

std::string const &
DefStmt::getName() const
{
    return *name;
}

std::vector<std::string const> const &
DefStmt::getArguments() const
{
    return args;
}

CompoundStmt const &
DefStmt::getBody() const
{
    return *body;
}
