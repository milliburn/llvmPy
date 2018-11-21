#include <llvmPy/AST.h>
#include <stdexcept>
using namespace llvmPy;
using std::ostream;
using std::string;
using std::stringstream;
using std::endl;

static constexpr int INDENT = 4;

static void
indentToStream(ostream &s, Stmt const &stmt, size_t indent)
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

AST::~AST() = default;

Expr::~Expr() = default;

Stmt::~Stmt() = default;

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

DecimalExpr::DecimalExpr(double value)
: _value(value)
{
}

double
DecimalExpr::getValue() const
{
    return _value;
}

void
DecimalExpr::toStream(std::ostream &s) const
{
    s << getValue() << 'd';
}

IntegerExpr::IntegerExpr(long value)
: _value(value)
{
}

int64_t
IntegerExpr::getValue() const
{
    return _value;
}

void
IntegerExpr::toStream(std::ostream &s) const
{
    s << getValue() << 'i';
}

IdentExpr::IdentExpr(std::string const &name)
: _name(name)
{
}

std::string const &
IdentExpr::getName() const
{
    return _name;
}

void
IdentExpr::toStream(std::ostream &s) const
{
    s << getName();
}

LambdaExpr::LambdaExpr(std::shared_ptr<Expr> const &expr)
: _expr(expr)
{
}

ArgNamesIter
LambdaExpr::args() const
{
    return { arg_begin(), arg_end() };
}

void
LambdaExpr::addArgument(std::string const &name)
{
    _args.emplace_back(name);
}

std::string const *
LambdaExpr::arg_begin() const
{
    return _args.data();
}

std::string const *
LambdaExpr::arg_end() const
{
    return _args.data() + _args.size();
}

Expr const &
LambdaExpr::getExpr() const
{
    assert(_expr);
    return *_expr;
}

void
LambdaExpr::toStream(std::ostream &s) const
{
    s << "(lambda";

    int iArg = 0;
    for (auto const &arg : args()) {
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
: _lhs(lhs), _rhs(rhs), _operator(op)
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
    assert(_lhs);
    return *_lhs;
}

Expr const &
BinaryExpr::getRightOperand() const
{
    assert(_rhs);
    return *_rhs;
}

TokenType
BinaryExpr::getOperator() const
{
    return _operator;
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
    for (auto const &arg : args()) {
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
: _value(value)
{
}

std::string const &
StringExpr::getValue() const
{
    return _value;
}

CallExpr::CallExpr(std::shared_ptr<Expr> const &callee)
: _callee(callee)
{
}

Expr const &
CallExpr::getCallee() const
{
    assert(_callee);
    return *_callee;
}

std::vector<std::shared_ptr<Expr const>> const &
CallExpr::getArguments() const
{
    return _arguments;
}

void
CallExpr::addArgument(std::shared_ptr<Expr const> argument)
{
    _arguments.emplace_back(argument);
}

TupleExpr::TupleExpr()
{
}

void
TupleExpr::toStream(std::ostream &s) const
{
    s << "(";

    int i = 0;
    for (auto const &member : _members) {
        if (i > 0) {
            s << ", ";
        }

        s << *member;
        i += 1;
    }

    if (_members.size() == 1) {
        s << ",";
    }

    s << ")";
}

std::vector<std::shared_ptr<Expr const>> const &
TupleExpr::getMembers() const
{
    return _members;
}

void
TupleExpr::addMember(std::shared_ptr<Expr> member)
{
    _members.emplace_back(member);
}

TokenExpr::TokenExpr(TokenType type)
: _tokenType(type)
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
    return _tokenType;
}

UnaryExpr::UnaryExpr(TokenType op, std::shared_ptr<Expr const> const &expr)
: _operator(op), _expr(expr)
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
    return _operator;
}

Expr const &
UnaryExpr::getExpr() const
{
    assert(_expr);
    return *_expr;
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
    return _statements;
}

void
CompoundStmt::addStatement(std::shared_ptr<Stmt const> const &stmt)
{
    _statements.push_back(stmt);
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
: _condition(condition),
  _thenBranch(thenBranch),
  _elseBranch(elseBranch)
{
    assert(this->_condition);
    assert(this->_thenBranch);
    assert(this->_elseBranch);
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
    assert(_condition);
    return *_condition;
}

Stmt const &
ConditionalStmt::getThenBranch() const
{
    assert(_thenBranch);
    return *_thenBranch;
}

Stmt const &
ConditionalStmt::getElseBranch() const
{
    assert(_elseBranch);
    return *_elseBranch;
}

ExprStmt::ExprStmt(std::shared_ptr<Expr const> const &expr)
: _expr(expr)
{
}

Expr const &
ExprStmt::getExpr() const
{
    assert(_expr);
    return *_expr;
}

DefStmt::DefStmt(
        std::string const &name,
        std::shared_ptr<Stmt const> const &body)
: _name(name),
  _body(body)
{
}

ReturnStmt::ReturnStmt(std::shared_ptr<Expr const> const &expr)
: _expr(expr)
{
}

Expr const &
ReturnStmt::getExpr() const
{
    assert(_expr);
    return *_expr;
}

std::shared_ptr<Expr const> const &
ReturnStmt::getExprPtr() const
{
    assert(_expr);
    return _expr;
}

Expr::Expr() = default;

Stmt::Stmt() = default;

WhileStmt::WhileStmt(
        std::shared_ptr<Expr const> const &condition,
        std::shared_ptr<Stmt const> const &body)
: _condition(condition),
  _body(body)
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
    assert(_condition);
    return *_condition;
}

Stmt const &
WhileStmt::getBody() const
{
    assert(_body);
    return *_body;
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
: _name(name),
  _value(value)
{
}

std::string const &
AssignStmt::getName() const
{
    return _name;
}

Expr const &
AssignStmt::getValue() const
{
    assert(_value);
    return *_value;
}

std::shared_ptr<Expr const> const &
AssignStmt::getValuePtr() const
{
    assert(_value);
    return _value;
}

std::string const &
DefStmt::getName() const
{
    return _name;
}

std::string const *
DefStmt::arg_begin() const
{
    return _args.data();
}

std::string const *
DefStmt::arg_end() const
{
    return _args.data() + _args.size();
}

ArgNamesIter
DefStmt::args() const
{
    return { arg_begin(), arg_end() };
}

Stmt const &
DefStmt::getBody() const
{
    assert(_body);
    return *_body;
}

void
DefStmt::addArgument(std::string const &name)
{
    _args.emplace_back(name);
}

std::shared_ptr<Expr const> const &
LambdaExpr::getExprPtr() const
{
    assert(_expr);
    return _expr;
}
