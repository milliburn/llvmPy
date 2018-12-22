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

LambdaExpr::LambdaExpr(Expr &expr)
: _expr(&expr)
{
    setExpr(expr);
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

BinaryExpr::BinaryExpr(Expr &lhs, TokenType op, Expr &rhs)
: _operator(op)
{
    setLeftOperand(lhs);
    setRightOperand(rhs);
}

void
BinaryExpr::toStream(std::ostream &s) const
{
    s << '(' << getLeftOperand()
      << ' ' << Token(getOperator())
      << ' ' << getRightOperand() << ')';
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
    for (auto const &arg : args()) {
        if (iArg > 0) {
            s << ", ";
        }

        s << arg;
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

CallExpr::CallExpr(Expr &callee)
{
    setCallee(callee);
}

TupleExpr::TupleExpr()
{
}

void
TupleExpr::toStream(std::ostream &s) const
{
    s << "(";

    int i = 0;
    for (auto const &member : members()) {
        if (i > 0) {
            s << ", ";
        }

        s << member;
        i += 1;
    }

    if (_members.size() == 1) {
        s << ",";
    }

    s << ")";
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

UnaryExpr::UnaryExpr(TokenType op, Expr &expr)
: _operator(op)
{
    setExpr(expr);
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

CompoundStmt::CompoundStmt()
{
}

void
CompoundStmt::toStream(std::ostream &s) const
{
    for (auto const &stmt : statements()) {
        s << stmt;
    }
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
        Expr &condition,
        Stmt &thenBranch,
        Stmt &elseBranch)
{
    setCondition(condition);
    setThenBranch(thenBranch);
    setElseBranch(elseBranch);
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

ExprStmt::ExprStmt(Expr &expr)
{
    setExpr(expr);
}

DefStmt::DefStmt(std::string const &name, Stmt &body)
: _name(name)
{
    setBody(body);
}

ReturnStmt::ReturnStmt(Expr &expr)
: _expr(&expr)
{
}

Expr::Expr() = default;

Stmt::Stmt() = default;

WhileStmt::WhileStmt(Expr &condition, Stmt &body)
{
    setCondition(condition);
    setBody(body);
}

void
WhileStmt::toStream(std::ostream &s) const
{
    s << "while " << getCondition() << ":" << endl;
    indentToStream(s, getBody(), INDENT);
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

AssignStmt::AssignStmt(std::string const &name, Expr &value)
: _name(name)
{
    setValue(value);
}

std::string const &
AssignStmt::getName() const
{
    return _name;
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

void
DefStmt::addArgument(std::string const &name)
{
    _args.emplace_back(name);
}

void
GetattrExpr::toStream(std::ostream &s) const
{
    s << getObject() << '.' << getName();
}

GetattrExpr::GetattrExpr(Expr &object, std::string const &name)
: _name(name)
{
    setObject(object);
}

std::string const &
GetattrExpr::getName() const
{
    return _name;
}

bool
AST::hasParent() const
{
    return _parent;
}

AST &
AST::getParent()
{
    assert(hasParent());
    return *_parent;
}

AST const &
AST::getParent() const
{
    assert(hasParent());
    return *_parent;
}

void
AST::setParent(AST &ast)
{
    assert(!hasParent());
    _parent = &ast;
}

void
AST::setParent(AST *ast)
{
    assert(ast);
    setParent(*ast);
}

void
AST::removeParent()
{
    _parent = nullptr;
}
