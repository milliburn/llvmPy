#include <llvmPy/Parser/ExprParser.h>
#include <llvm/Support/Casting.h>
using namespace llvmPy;
using llvm::dyn_cast;

static std::map<TokenType, int>
initPrecedence()
{
    // Follows the table at:
    // https://docs.python.org/3/reference/expressions.html#operator-precedence
    // (where lower precedences are at the top).
    std::map<TokenType, int> map;

    map[kw_lambda] = 1;

    map[tok_lt] = 6;
    map[tok_lte] = 6;
    map[tok_eq] = 6;
    map[tok_neq] = 6;
    map[tok_gt] = 6;
    map[tok_gte] = 6;

    map[tok_add] = 11;
    map[tok_sub] = 11;

    map[tok_mul] = 12;
    map[tok_div] = 12;

    map[tok_comma] = 17; // Tuple binding

    return map;
}

std::unique_ptr<Expr>
ExprParser::fromIter(
        ExprParser::TTokenIter &iter,
        ExprParser::TTokenIter end)
{
    ExprParser parser(iter, std::move(end));
    return parser.parse();
}

ExprParser::ExprParser(
        ExprParser::TTokenIter &iter,
        ExprParser::TTokenIter end)
: iter(iter), iter_end(end), precedences(initPrecedence())
{
}

std::unique_ptr<Expr>
ExprParser::parse()
{
    Expr *result = parseImpl(0, nullptr);
    return std::unique_ptr<Expr>(result);
}

Expr *
ExprParser::parseImpl(int lastPrec, Expr *lhs)
{
    if (end()) {
        return lhs;
    }

    Expr *rhs = nullptr;
    TokenExpr *op = nullptr;
    int curPrec = 0;

    if ((op = findOperator())) {
        curPrec = getPrecedence(op);
        if (curPrec <= lastPrec) {
            back();
            return lhs;
        }
    }

    if ((rhs = findIntegerLiteral()) || (rhs = findIdentifier())) {
    }

    rhs = parseImpl(curPrec, rhs);

    if (!lhs && !op) {
        return rhs;
    } else if (lhs && !op) {
        assert(!rhs);
        return lhs;
    } else if (!lhs && op) {
        int sign = 0;

        switch (op->getTokenType()) {
        case tok_sub:
            sign = -1;
            // FALLTHROUGH
        case tok_add:
            if (auto *literal = dyn_cast<IntLitExpr>(rhs)) {
                if (sign < 0) {
                    auto value = literal->getValue();
                    delete literal;
                    literal = new IntLitExpr(-value);
                }

                return literal;
            } else {
                // FALLTHROUGH
            }
        default:
            throw "Not implemented";
        }


        // This can happen with numbers (e.g. +1, -2).

        return nullptr;
    } else {
        Expr *binop = new BinaryExpr(lhs, op->getTokenType(), rhs);
        return parseImpl(lastPrec, binop);
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

bool
ExprParser::end()
{
    return iter == iter_end || (*iter).type == tok_eof;
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
    // Sign is applied separately.

    if (is(tok_number)) {
        std::string const *text = token().str;
        int64_t value = atol(text->c_str());
        next();
        return new IntLitExpr(value);
    }

    return nullptr;
}

IdentExpr *
ExprParser::findIdentifier()
{
    if (is(tok_ident)) {
        auto *expr = new IdentExpr(token().str);
        next();
        return expr;
    }

    return nullptr;
}

TokenExpr *
ExprParser::findOperator()
{
    if (is_a(tok_oper)) {
        TokenType tokenType = token().type;
        next();
        return new TokenExpr(tokenType);
    }

    return nullptr;
}

int
ExprParser::getPrecedence(TokenType tokenType) const
{
    if (precedences.count(tokenType)) {
        return precedences.at(tokenType);
    } else {
        return 0;
    }
}

int
ExprParser::getPrecedence(TokenExpr *tokenExpr) const
{
    return getPrecedence(tokenExpr->getTokenType());
}
