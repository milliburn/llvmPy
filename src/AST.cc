#include <llvmPy/AST.h>
#include <stdexcept>
using namespace llvmPy;
using std::ostream;
using std::string;
using std::stringstream;

static constexpr char eof = (char) std::istream::traits_type::eof();

static void
indentToStream(ostream &s, Stmt const &stmt, int indent)
{
    stringstream ss;
    ss << stmt;
    ss.seekg(0, std::ios::beg);

    bool indentNext = true;
    while (true) {
        if (ss.eof()) break;
        if (indentNext) {
            for (int i = 0; i < indent; ++i) {
                s << ' ';
            }
            indentNext = false;
        }

        char ch = (char) ss.get();
        if (ch == eof) break;
        s << ch;
        if (ch == '\n') {
            indentNext = true;
        }
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
AST::toStream(std::ostream &) const
{
    throw std::runtime_error("Not Implemented");
}

void
StrLitExpr::toStream(std::ostream & s) const
{
    s << '"' << getValue() << '"';
}

void
DecLitExpr::toStream(std::ostream & s) const
{
    s << value << 'd';
}

void
IntLitExpr::toStream(std::ostream & s) const
{
    s << value << 'i';
}

void
IdentExpr::toStream(std::ostream & s) const
{
    s << name;
}

void
LambdaExpr::toStream(std::ostream & s) const
{
    s << "(lambda";

    for (int i = 0; i < args.size(); ++i) {
        if (i > 0) s << ',';
        s << ' ' << args[i];
    }

    s << ": " << expr << ')';
}

void
BinaryExpr::toStream(std::ostream & s) const
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
ExprStmt::toStream(std::ostream & s) const
{
    s << expr;
}

void
AssignStmt::toStream(std::ostream & s) const
{
    s << lhs << " = " << rhs;
}

void
ImportStmt::toStream(std::ostream & s) const
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

    s << "):\n";

    for (int i = 0; i < stmts.size(); ++i) {
        if (i > 0) s << '\n';
        indentToStream(s, *stmts[i], 2);
    }
}

void
ReturnStmt::toStream(std::ostream &s) const
{
    s << "return " << expr << '\n';
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
CallExpr::addArgument(std::unique_ptr<Expr> arg)
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
    AST::toStream(s);
}

std::vector<std::unique_ptr<Expr const>> const &
TupleExpr::getMembers()
{
    return members;
}

void
TupleExpr::addMember(std::unique_ptr<Expr> member)
{

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
