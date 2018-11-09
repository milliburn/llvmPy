#include <llvmPy/Parser/ExprParser.h>
using namespace llvmPy;

std::unique_ptr<Expr>
ExprParser::fromIter(ExprParser::TTokenIter iter)
{
    ExprParser parser(iter);
    return parser.parse();
}

ExprParser::ExprParser(ExprParser::TTokenIter iter)
: iter(iter)
{
}

std::unique_ptr<Expr>
ExprParser::parse()
{
    consume();
    std::unique_ptr<Expr> result(output.back());
    return result;
}

void
ExprParser::consume()
{
    if (auto lit = findIntegerLiteral()) {
        output.push_back(lit);
    } else if (auto ident = findIdentifier()) {
        output.push_back(ident);
    } else if (auto token = findOperator()) {
        operators.push_back(token);
    }
}

bool
ExprParser::is(TokenType tokenType)
{
    return token().type == tokenType;
}

bool
ExprParser::is_a(TokenType tokenType)
{
    return token().type & tokenType;
}

void
ExprParser::next()
{
    iter++;
}

void
ExprParser::back()
{
    iter--;
}

Token const &
ExprParser::token() const
{
    return *iter;
}

IntLitExpr *
ExprParser::findIntegerLiteral()
{
    int sign = 0;

    if (is(tok_add)) {
        sign = 1;
        next();
    } else if (is(tok_sub)) {
        sign = -1;
        next();
    }

    if (is(tok_number)) {
        std::string const *text = token().str;
        int64_t value = atol(text->c_str());
        next();
        return new IntLitExpr(sign < 0 ? -value : value);
    }

    back();
    return nullptr;
}

IdentExpr *
ExprParser::findIdentifier()
{
    if (is(tok_ident)) {
        return new IdentExpr(token().str);
    }

    return nullptr;
}

TokenExpr *
ExprParser::findOperator()
{
    if (is_a(tok_oper)) {
        TokenType tokenType = token().type;
        return new TokenExpr(tokenType);
    }

    return nullptr;
}
