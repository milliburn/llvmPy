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
    Expr *result = readExpr(0, nullptr);
    if (!result) result = new TupleExpr();
    return std::unique_ptr<Expr>(result);
}

Expr *
ExprParser::readSubExpr()
{
    TokenType term = tok_unknown;

    if (is(tok_lp)) {
        next();
        term = tok_rp;
    }

    TupleExpr *tuple = nullptr;

    for (;;) {
        auto *expr = readExpr(0, nullptr);
        bool isTerminal = false;

        if (end() || is(tok_colon)) {
            isTerminal = true;
        } else if (term && is(term)) {
            next();
            isTerminal = true;
        }

        if (isTerminal) {
            if (tuple) {
                if (expr) {
                    tuple->addMember(std::unique_ptr<Expr>(expr));
                }

                return tuple;
            } else if (!expr) {
                return new TupleExpr();
            } else {
                return expr;
            }
        } else if (is(tok_comma)) {
            // This subexpression is a tuple.
            next();

            if (!tuple) {
                tuple = new TupleExpr();
            }

            tuple->addMember(std::unique_ptr<Expr>(expr));
        }
    }
}

Expr *
ExprParser::readExpr(int lastPrec, Expr *lhs)
{
    Expr *rhs = nullptr;
    TokenExpr *op = nullptr;
    int curPrec = 0;

    if (end() || is(tok_rp)) {
        // End of (sub)expression.
        // Terminator (if any) is intentionally left un-consumed.
        return lhs;
    } else if (is(tok_colon)) {
        // The colon is used in lambda-expressions and
        // as a statement terminator.
        return lhs;
    } else if (is(tok_comma)) {
        // Comma is a delimiter (no binding) between expressions.
        // It is intentionally left un-consumed.
        return lhs;
    } else if ((op = findOperator())) {
        curPrec = getPrecedence(op);

        if (curPrec <= lastPrec) {
            back();
            return lhs;
        }
    }

    if (is(tok_lp)) {
        rhs = readSubExpr();
        rhs = readExpr(curPrec, rhs);
    } else if (
            (rhs = findNumericLiteral()) ||
            (rhs = findStringLiteral()) ||
            (rhs = findIdentifier()) ||
            (rhs = findLambdaExpr())) {
        rhs = readExpr(
                llvm::isa<LambdaExpr>(rhs)
                        ? getPrecedence(kw_lambda)
                        : curPrec,
                rhs);
    }

    if (lhs && op && rhs) {
        // Apply binary operator to LHS and RHS.
        Expr *binop = new BinaryExpr(lhs, op->getTokenType(), rhs);
        return readExpr(lastPrec, binop);
    } else if (lhs && !op && rhs) {
        // Function call (apply arguments in RHS to callee in LHS).
        return buildCall(lhs, rhs);
    } else if (lhs && !op && !rhs) {
        // Independent expression (generally literal or identifier).
        return lhs;
    } else if (!lhs && op && rhs) {
        // Special case of an independent expression such as -1, which
        // normally would be interpreted as a binary subtraction.
        // TODO: This should be parsed as an unary operation instead.

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
            } else if (auto *literal = dyn_cast<DecLitExpr>(rhs)) {
                if (sign < 0) {
                    auto value = literal->getValue();
                    delete literal;
                    literal = new DecLitExpr(-value);
                }

                return literal;
            } else {
                // FALLTHROUGH
            }
        default:
            throw "Not implemented";
        }
    } else if (!lhs && !op) {
        return rhs;
    } else {
        throw "Not implemented";
    }
}

CallExpr *
ExprParser::buildCall(Expr *lhs, Expr *rhs)
{
    auto *call = new CallExpr(std::unique_ptr<Expr>(lhs));

    if (auto *tuple = dyn_cast<TupleExpr>(rhs)) {
        for (auto &member : tuple->getMembers()) {
            call->addArgument(std::move(member));
        }

        delete(tuple);
    } else if (rhs) {
        call->addArgument(std::unique_ptr<Expr>(rhs));
    }

    return call;
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
    if (!end()) {
        iter++;
    }
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

Expr *
ExprParser::findNumericLiteral()
{
    int sign = 0;

    // TODO: Sign should be parsed as an unary operation instead.
    if (is(tok_add)) {
        next();
        sign = 1;
    } else if (is(tok_sub)) {
        next();
        sign = -1;
    }

    if (is(tok_number)) {
        std::string const *text = token().str;
        next();

        if (text->find('.') != std::string::npos) {
            double value = atof(text->c_str());
            return new DecLitExpr(sign < 0 ? -value : value);
        } else {
            int64_t value = atol(text->c_str());
            return new IntLitExpr(sign < 0 ? -value : value);
        }
    } else {
        if (sign != 0) {
            back();
        }

        return nullptr;
    }
}

StrLitExpr *
ExprParser::findStringLiteral()
{
    if (is(tok_string)) {
        // The substring removes string delimiters (" or ').
        std::string const *str = token().str;
        auto value = std::make_unique<std::string const>(
                str->substr(1, str->length() - 2));
        next();
        return new StrLitExpr(std::move(value));
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

LambdaExpr *
ExprParser::findLambdaExpr()
{
    if (is(kw_lambda)) {
        next();

        std::vector<std::string const> argNames;

        Expr *argsSubExpr = readSubExpr();

        if (auto *tuple = dyn_cast<TupleExpr>(argsSubExpr)) {
            for (auto const &member : tuple->getMembers()) {
                auto const &ident = llvm::cast<IdentExpr>(*member);
                argNames.push_back(ident.getName());
            }
        } else if (auto *ident = dyn_cast<IdentExpr>(argsSubExpr)) {
            argNames.push_back(ident->getName());
        } else {
            assert(false && "Invalid argument subexpression");
        }

        delete(argsSubExpr);

        assert(is(tok_colon));
        next();

        Expr *lambdaBody = readSubExpr();

        return new LambdaExpr(argNames, lambdaBody);
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
