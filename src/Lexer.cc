#include <llvmPy/Lexer.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
using namespace llvmPy;
using namespace std;

static constexpr char eof = static_cast<char>(std::istream::traits_type::eof());

static std::map<std::string const, TokenType>
buildKeywordMap()
{
    std::map<std::string const, TokenType> kws;
    kws["lambda"] = kw_lambda;
    kws["def"] = kw_def;
    kws["return"] = kw_return;
    kws["if"] = kw_if;
    kws["elif"] = kw_elif;
    kws["else"] = kw_else;
    kws["pass"] = kw_pass;
    kws["while"] = kw_while;
    kws["break"] = kw_break;
    kws["continue"] = kw_continue;
    return kws;
}

Lexer::Lexer(std::istream & stream)
: _stream(stream), _keywords(buildKeywordMap())
{
    _ibuf = 0;
    _ilast = -1;
    _line = 0;
    _col = 0;
    _error = "";
}

bool
Lexer::tokenize(std::vector<Token> & out)
{
    _tokens.clear();

    bool newline = true;
    bool comment = false;
    size_t indent = 0;

    while (true) {
        reset();

        // Skip preceding whitespace, but record the indent level.
        while (is(' ') || is('\t')) {
            indent++;
        }

        if (is('#')) {
            // Comments span until the end of line.
            comment = true;
        }

        if (is(eof)) {
            add(Token(tok_eof));
            comment = false;
            break;
        } else if (oneof("\r\n")) {
            while (oneof("\r\n")); // Contract newlines.
            add(Token(tok_eol));
            newline = true;
            comment = false;
            indent = 0;
            continue; // Tokens can't span lines.
        } else if (comment) {
            next();
            continue;
        }

        if (newline) {
            add(Token(tok_indent, indent));
            newline = false;
        }

        if (strlit()) continue;
        if (numlit()) continue;
        if (syntax()) continue;
        if (ident()) continue;
    }

    out = std::move(_tokens);
    return true;
}

bool
Lexer::isError()
{
    return false;
}

long
Lexer::getLine()
{
    return 0;
}

long
Lexer::getColumn()
{
    return 0;
}

std::string
Lexer::getError()
{
    return std::string();
}

bool
Lexer::isEof()
{
    return _ch == eof || _stream.eof();
}

void
Lexer::add(Token token)
{
    if (token.getTokenType() == tok_eol) {
        // Ignore empty lines at start of file.
        if (_tokens.empty()) return;

        // Ignore empty lines in the middle of the file.
        if (_tokens.back().getTokenType() == tok_eol) return;
    } else if (token.getTokenType() == tok_eof) {
        // Ignore empty lines at end of file.
        if (!_tokens.empty() && _tokens.back().getTokenType() == tok_eol) {
            _tokens.pop_back();
        }
    }

    _tokens.emplace_back(std::move(token));
}

void
Lexer::next()
{
    if (_ibuf < _ilast - 1) {
        _ch = _buf[++_ibuf];
    } else if (_ibuf == _ilast - 1) {
        ++_ibuf;
        _ch = static_cast<char>(_stream.peek());
        _ilast = -1;
    } else {
        if (_ilast > -1) {
            // Don't touch the buffer unless a state was pushed.
            // Very long tokens (e.g. strings) will have their own
            // buffering mechanism.
            _buf[_ibuf++] = _ch;
        }

        _stream.get();
        _ch = static_cast<char>(_stream.peek());
    }
}

void
Lexer::next(std::stringstream & ss)
{
    ss << _ch;
    next();
}

void
Lexer::reset()
{
    _ibuf = 0;
    _ilast = -1;
    _ch = static_cast<char>(_stream.peek());
}

void
Lexer::push()
{
    _ilast = _ibuf;
}

void
Lexer::pop()
{
    auto tmp = _ibuf;
    _ibuf = _ilast;
    _ilast = tmp;

    if (_ibuf == _ilast) {
        _ch = static_cast<char>(_stream.peek());
    } else {
        _ch = _buf[_ibuf];
    }
}

bool
Lexer::is(char c)
{
    if (_ch == c) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Lexer::is(char c, std::stringstream & ss)
{
    char cc = _ch;

    if (is(c)) {
        ss << cc;
        return true;
    } else {
        return false;
    }
}

bool
Lexer::oneof(char const * chs)
{
    if (strchr(chs, _ch)) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Lexer::oneof(char const * chs, std::stringstream & ss)
{
    char const * ptr = strchr(chs, _ch);
    if (ptr) {
        ss << *ptr;
        next();
        return true;
    } else {
        return false;
    }
}

bool
Lexer::oneof(int (* cls)(int))
{
    if (cls(_ch)) {
        next();
        return true;
    }

    return false;
}

bool
Lexer::oneof(int (* cls)(int), std::stringstream & ss)
{
    char cc = _ch;

    if (oneof(cls)) {
        ss << cc;
        return true;
    } else {
        return false;
    }
}

bool
Lexer::numlit()
{
    bool point = false;

    push();
    stringstream ss;

    if (is('.', ss))
        point = true;

    if (!oneof(isdigit, ss)) {
        pop();
        return false;
    }

    while (oneof(isdigit, ss));

    if (!point && is('.', ss)) {
        while (oneof(isdigit, ss));
    }

    add(Token(
            tok_number,
            std::make_unique<std::string>(ss.str())));

    return true;
}

bool
Lexer::strlit()
{
    char end;

    if      (is('\'')) end = '\'';
    else if (is('"'))  end = '"';
    else return false;

    stringstream ss;
    ss << end;

    do {
        if (is('\\', ss)) {
            // Skip over end of string. If it's anything other
            // than the delimiter, include it as part of the string.
            is(end, ss);
        }

        if (is(end, ss)) {
            break;
        } else {
            next(ss);
        }
    } while (true);

    add(Token(
            tok_string,
            std::make_unique<std::string>(ss.str())));


    return true;
}

bool
Lexer::ident()
{
    ssize_t start = _ibuf;

    if (!oneof(isalpha) && !is('_'))
        return false;

    while (oneof(isalnum) || is('_'));

    auto str = std::make_unique<std::string>(&_buf[start], _ibuf - start);

    if (auto kw = getKeyword(*str)) {
        add(Token(kw));
    } else {
        add(Token(tok_ident, std::move(str)));
    }

    return true;
}

bool
Lexer::syntax()
{
    char a = _ch;
    if (oneof("<>!=+-*/")) {
        size_t t;

        switch (a) {
        case '<': t = tok_lt; break;
        case '>': t = tok_gt; break;
        case '!': t = tok_not; break;
        case '=': t = tok_assign; break;
        case '+': t = tok_add; break;
        case '-': t = tok_sub; break;
        case '*': t = tok_mul; break;
        case '/': t = tok_div; break;
        default: assert(false);
        }

        if (is('=')) {
            if (a == '=') {
                t = tok_eq;
            } else if (a == '!') {
                t = tok_neq;
            } else if (strchr("<>", a)) {
                t |= tok_cmpeq;
            } else {
                t |= tok_assign;
            }
        }

        add(Token(static_cast<TokenType>(t)));
        return true;
    } else if (oneof(".,:;()[]")) {
        int t;

        switch (a) {
        case '.': t = tok_dot; break;
        case ',': t = tok_comma; break;
        case ':': t = tok_colon; break;
        case ';': t = tok_semicolon; break;
        case '(': t = tok_lp; break;
        case ')': t = tok_rp; break;
        case '[': t = tok_lb; break;
        case ']': t = tok_rb; break;
        default: assert(false);
        }

        add(Token(static_cast<TokenType>(t)));
        return true;
    }

    return false;
}

TokenType
Lexer::getKeyword(std::string const &kw)
{
    if (_keywords.count(kw)) {
        return _keywords.at(kw);
    } else {
        return tok_unknown;
    }
}
