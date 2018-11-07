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

void
AST::toStream(std::ostream &) const
{
    throw std::runtime_error("Not Implemented");
}

void
StrLitExpr::toStream(std::ostream & s) const
{
    s << '"' << str << '"';
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
    s << lhs << '(';

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

