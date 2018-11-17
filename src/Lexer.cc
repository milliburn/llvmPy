#include <llvmPy/Lexer.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
using namespace llvmPy;
using namespace std;

static constexpr int eof = std::istream::traits_type::eof();

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
: stream(stream), keywords(buildKeywordMap())
{
    ibuf = 0;
    ilast = -1;
    line = 0;
    col = 0;
    error = "";
}

bool
Lexer::tokenize(std::vector<Token> & out)
{
    tokens.clear();

    bool newline = true;
    bool comment = false;
    int indent = 0;

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

        if (is((char) eof)) {
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

    out = std::move(tokens);
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
    return ch == (char) eof || stream.eof();
}

void
Lexer::add(Token token)
{
    if (token.getTokenType() == tok_eol) {
        // Ignore empty lines at start of file.
        if (tokens.empty()) return;

        // Ignore empty lines in the middle of the file.
        if (tokens.back().getTokenType() == tok_eol) return;
    } else if (token.getTokenType() == tok_eof) {
        // Ignore empty lines at end of file.
        if (!tokens.empty() && tokens.back().getTokenType() == tok_eol) {
            tokens.pop_back();
        }
    }

    tokens.emplace_back(std::move(token));
}

void
Lexer::next()
{
    if (ibuf < ilast - 1) {
        ch = buf[++ibuf];
    } else if (ibuf == ilast - 1) {
        ++ibuf;
        ch = (char) stream.peek();
        ilast = -1;
    } else {
        if (ilast > -1) {
            // Don't touch the buffer unless a state was pushed.
            // Very long tokens (e.g. strings) will have their own
            // buffering mechanism.
            buf[ibuf++] = ch;
        }

        stream.get();
        ch = (char) stream.peek();
    }
}

void
Lexer::next(std::stringstream & ss)
{
    ss << ch;
    next();
}

void
Lexer::reset()
{
    ibuf = 0;
    ilast = -1;
    ch = (char) stream.peek();
}

void
Lexer::push()
{
    ilast = ibuf;
}

void
Lexer::pop()
{
    auto tmp = ibuf;
    ibuf = ilast;
    ilast = tmp;

    if (ibuf == ilast) {
        ch = (char) stream.peek();
    } else {
        ch = buf[ibuf];
    }
}

bool
Lexer::is(char c)
{
    if (ch == c) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Lexer::is(char c, std::stringstream & ss)
{
    char cc = ch;

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
    if (strchr(chs, ch)) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Lexer::oneof(char const * chs, std::stringstream & ss)
{
    char const * ptr = strchr(chs, ch);
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
    if (cls(ch)) {
        next();
        return true;
    }

    return false;
}

bool
Lexer::oneof(int (* cls)(int), std::stringstream & ss)
{
    char cc = ch;

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
    int start = ibuf;

    if (!oneof(isalpha) && !is('_'))
        return false;

    while (oneof(isalnum) || is('_'));

    auto str = std::make_unique<std::string>(&buf[start], ibuf - start);

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
    char a = ch;
    if (oneof("<>!=+-*/")) {
        int t;

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
    if (keywords.count(kw)) {
        return keywords.at(kw);
    } else {
        return tok_unknown;
    }
}
