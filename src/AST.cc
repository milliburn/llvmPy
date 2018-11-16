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
    s << getValue() << 'd';
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
    s << getValue() << 'i';
}

IdentExpr::IdentExpr(std::string const &name)
: name(name)
{
}

std::string const &
IdentExpr::getName() const
{
    return name;
}

void
IdentExpr::toStream(std::ostream &s) const
{
    s << getName();
}

LambdaExpr::LambdaExpr(std::shared_ptr<Expr> const &expr)
: expr(expr)
{
}

std::vector<std::string const> const &
LambdaExpr::getArguments() const
{
    return arguments;
}

void
LambdaExpr::addArgument(std::string const &name)
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

    int iArg = 0;
    for (auto const &arg : getArguments()) {
        if (iArg > 0) {
            s << ",";
        }

        s << ' ' << arg;
        iArg += 1;
    }

    s << ": " << getExpr() << ')';
}

BinaryExpr::BinaryExpr(
        std::shared_ptr<Expr const> const &lhs,
        TokenType op,
        std::shared_ptr<Expr const> const &rhs)
: lhs(lhs), rhs(rhs), op(op)
{
}

void
BinaryExpr::toStream(std::ostream &s) const
{
    s << '(' << getLeftOperand()
      << ' ' << Token(getOperator())
      << ' ' << getRightOperand() << ')';
}

Expr const &
BinaryExpr::getLeftOperand() const
{
    return *lhs;
}

Expr const &
BinaryExpr::getRightOperand() const
{
    return *rhs;
}

TokenType
BinaryExpr::getOperator() const
{
    return op;
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
    s << getExpr() << endl;
}

void
AssignStmt::toStream(std::ostream &s) const
{
    s << getName() << " = " << getValue() << endl;
}

void
DefStmt::toStream(std::ostream &s) const
{
    s << "def " << getName() << "(";

    int iArg = 0;
    for (auto const &arg : getArguments()) {
        if (iArg > 0) {
            s << ", ";
        }

        s << arg;
        iArg += 1;
    }

    s << "):" << endl;

    indentToStream(s, getBody(), INDENT);
}

void
ReturnStmt::toStream(std::ostream &s) const
{
    s << "return " << getExpr() << endl;
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

StringExpr::StringExpr(std::string const &value)
: value(value)
{
}

std::string const &
StringExpr::getValue() const
{
    return value;
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

UnaryExpr::UnaryExpr(TokenType op, std::shared_ptr<Expr const> const &expr)
: op(op), expr(expr)
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

std::vector<std::shared_ptr<Stmt const>> const &
CompoundStmt::getStatements() const
{
    return statements;
}

void
CompoundStmt::addStatement(std::shared_ptr<Stmt const> const &stmt)
{
    statements.push_back(stmt);
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
        std::shared_ptr<Expr const> const &condition,
        std::shared_ptr<Stmt const> const &thenBranch,
        std::shared_ptr<Stmt const> const &elseBranch)
: condition(condition),
  thenBranch(thenBranch),
  elseBranch(elseBranch)
{
    assert(this->condition);
    assert(this->thenBranch);
    assert(this->elseBranch);
}

void
ConditionalStmt::toStream(std::ostream &s) const
{
    s << "if " << getCondition() << ":" << endl;
    indentToStream(s, getThenBranch(), INDENT);

    if (getElseBranch().isa<ConditionalStmt>()) {
        // XXX: This relies on toStream() not prepending anything, i.e. we get
        // XXX: the "el" from here and the "if" from the next toStream().
        s << "el";
        s << getElseBranch();
    } else if (!getElseBranch().isa<PassStmt>()) {
        s << "else:" << endl;
        indentToStream(s, getElseBranch(), INDENT);
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

ExprStmt::ExprStmt(std::shared_ptr<Expr const> const &expr)
: expr(expr)
{
}

Expr const &
ExprStmt::getExpr() const
{
    return *expr;
}

DefStmt::DefStmt(
        std::string const &name,
        std::shared_ptr<Stmt const> const &body)
: name(name),
  body(body)
{
}

ReturnStmt::ReturnStmt(std::shared_ptr<Expr const> const &expr)
: expr(expr)
{
}

Expr const &
ReturnStmt::getExpr() const
{
    return *expr;
}

std::shared_ptr<Expr const> const &
ReturnStmt::getExprPtr() const
{
    return expr;
}

Expr::Expr() = default;

Stmt::Stmt() = default;

WhileStmt::WhileStmt(
        std::shared_ptr<Expr const> const &condition,
        std::shared_ptr<Stmt const> const &body)
: condition(condition),
  body(body)
{
}

void
WhileStmt::toStream(std::ostream &s) const
{
    s << "while " << getCondition() << ":" << endl;
    indentToStream(s, getBody(), INDENT);
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
        std::string const &name,
        std::shared_ptr<Expr const> const &value)
: name(name),
  value(value)
{
}

std::string const &
AssignStmt::getName() const
{
    return name;
}

Expr const &
AssignStmt::getValue() const
{
    return *value;
}

std::shared_ptr<Expr const> const &
AssignStmt::getValuePtr() const
{
    return value;
}

std::string const &
DefStmt::getName() const
{
    return name;
}

std::vector<std::string const> const &
DefStmt::getArguments() const
{
    return args;
}

Stmt const &
DefStmt::getBody() const
{
    return *body;
}

void
DefStmt::addArgument(std::string const &name)
{
    args.emplace_back(name);
}

std::shared_ptr<Expr const> const &
LambdaExpr::getExprPtr() const
{
    return expr;
}
