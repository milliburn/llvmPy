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

    return result;
}

Stmt *
Parser4::Statement()
{
    return nullptr;
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

Expr *
Parser4::NumericLiteral()
{
    if (!peek(tok_number)) {
        return nullptr;
    }

    auto &text = take().getString();

    if (text.find('.') != std::string::npos) {
        double value = atof(text.c_str());
        return new DecimalExpr(value);
    } else {
        int64_t value = atol(text.c_str());
        return new IntegerExpr(value);
    }
}

Expr *
Parser4::StringLiteral()
{
    if (!peek(tok_string)) {
        return nullptr;
    }

    auto &text = take().getString();
    // Remove " or ' delimiters.
    auto value = text.substr(1, text.length() - 2);
    return new StringExpr(value);
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

    if (_index == _tokenCount - 1) {
        assert(peek(tok_eof));
    }

    return _tokens[_index];
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
