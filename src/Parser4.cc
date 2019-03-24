#include <llvmPy/Parser4.h>
#include <assert.h>
using namespace llvmPy;

static std::unordered_map<TokenType, int>
generatePrecedenceMap()
{
    // Follows the table at:
    // https://docs.python.org/3/reference/expressions.html#operator-precedence
    // (where lower precedences are at the top).
    std::unordered_map<TokenType, int> map;
    map[tok_comma] = -1;
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
    map[tok_dot] = 16;
    map[tok_lp] = 17;
    map[tok_rp] = 17;
    return map;
}

Parser4::Parser4(std::vector<Token> const &tokens)
    : _tokens(tokens.data()),
      _tokenCount(tokens.size()),
      _index(0),
      _precedence(generatePrecedenceMap())
{
}

bool
Parser4::EndOfFile()
{
    return is(tok_eof);
}

bool
Parser4::EndOfLine()
{
    return is(tok_eol);
}

TokenType
Parser4::UnaryOperator()
{
    auto &token = peek();
    auto type = token.getTokenType();

    if (type & tok_unary) {
        next();
        assert(_precedence.count(type));
        return type;
    } else {
        return tok_null;
    }
}

TokenType
Parser4::BinaryOperator()
{
    auto &token = peek();
    auto type = token.getTokenType();

    if (type & tok_binary) {
        next();
        assert(_precedence.count(type));
        return type;
    } else {
        return tok_null;
    }
}

Stmt *
Parser4::Statement()
{
    return nullptr;
}

Expr *
Parser4::Expression()
{
    return Expression(0);
}

Expr *
Parser4::Expression(int minimumPrecedence)
{
    Expr *result = nullptr;

    if (!result) result = NumericLiteral();
    if (!result) result = StringLiteral();
    if (!result) result = UnaryExpression();
    if (!result) result = Subexpression();
    if (!result) return nullptr;

    while (true) {
        if (EndOfLine()) {
            break;
        } else if (EndOfFile()) {
            break;
        } else if (auto operator_ = BinaryOperator()) {
            int operatorPrecedence = precedence(operator_);

            if (operatorPrecedence < minimumPrecedence) {
                back();
                break;
            }

            bool isLeftAssoc = isLeftAssociative(operator_);
            int nextPrecedence = operatorPrecedence + (isLeftAssoc ? 1 : 0);

            auto *rhs = Expression(nextPrecedence);
            assert(rhs && "Expected right-hand side of binary expression");
            result = new BinaryExpr(*result, operator_, *rhs);
        } else {
            break;
        }
    }

    return result;
}

Expr *
Parser4::Subexpression()
{
    if (is(tok_lp)) {
        Expr *result = Expression();
        assert(is(tok_rp) && "Expected ')'");
        if (!result) result = new TupleExpr();
        return result;
    } else {
        return nullptr;
    }
}

Expr *
Parser4::UnaryExpression()
{
    if (auto operator_ = UnaryOperator()) {
        auto *expr = Expression();
        assert(expr && "Expected unary expression");
        return new UnaryExpr(operator_, *expr);
    } else {
        return nullptr;
    }
}

Expr *
Parser4::NumericLiteral()
{
    if (peek(tok_number)) {
        auto &text = take().getString();

        if (text.find('.') != std::string::npos) {
            double value = atof(text.c_str());
            return new DecimalExpr(value);
        } else {
            int64_t value = atol(text.c_str());
            return new IntegerExpr(value);
        }
    } else {
        return nullptr;
    }
}

Expr *
Parser4::StringLiteral()
{
    if (peek(tok_string)) {
        auto &text = take().getString();
        // Remove " or ' delimiters.
        auto value = text.substr(1, text.length() - 2);
        return new StringExpr(value);
    } else {
        return nullptr;
    }
}

void
Parser4::next()
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    if (_index < _tokenCount - 1) {
        // Stop advancing once EOF is reached.
        _index++;
    }
}

void
Parser4::back()
{
    assert(_index > 0);
    assert(_index < _tokenCount);
    _index--;
}

bool
Parser4::is(TokenType const type)
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    auto &token = peek();

    if (token.getTokenType() == type) {
        next();
        return true;
    } else {
        return false;
    }
}

Token const &
Parser4::peek() const
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    auto &token = _tokens[_index];

    if (_index == _tokenCount - 1) {
        assert(token.getTokenType() == tok_eof);
    }

    return token;
}

bool
Parser4::peek(TokenType type) const
{
    return peek().getTokenType() == type;
}

Token const &
Parser4::take()
{
    auto &token = peek();
    next();
    return token;
}

int
Parser4::precedence(Token const &token) const
{
    return precedence(token.getTokenType());
}

int
Parser4::precedence(TokenType type) const
{
    assert(_precedence.count(type));
    return _precedence.at(type);
}

bool
Parser4::isLeftAssociative(Token const &token) const
{
    return isLeftAssociative(token.getTokenType());
}

bool
Parser4::isLeftAssociative(TokenType type) const
{
    return ! (type & tok_rassoc);
}
