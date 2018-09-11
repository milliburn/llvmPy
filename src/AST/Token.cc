#include <llvmPy/AST.h>
#include <llvmPy/AST/Lang.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <llvmPy/AST/Token.h>

using namespace llvmPy::AST;
using namespace std;

static constexpr int eof = std::istream::traits_type::eof();

static inline bool
iseof(int c)
{
    return c == eof;
}

static inline bool
iseol(int c)
{
    return c == '\n' || c == '\r';
}

static inline bool
issyntax(int c) {
    return strchr("()=#", c) || iseof(c);
}

static inline bool
isoper(int c) {
    return strchr("+-", c);
}

static inline bool
isident(int c) {
    return isalpha(c) || c == '_';
}

Token::Token(TokenType tokenType)
: tokenType(tokenType)
{
}

Tokenizer::Tokenizer(std::istream & stream)
: stream(stream)
{
    reset();
}

std::vector<Token *>
Tokenizer::tokenize()
{
    tokens.clear();

    bool newline = true;
    int indent = 0;

    do {
        reset();

        // Skip preceding whitespace, but record the indent level.
        while (is(' ') || is('\t')) {
            indent++;
        }

        if (is('#')) {
            // Comments span until the end of line.
            while (!(is((char) eof) || oneof("\r\n")));
        }

        if (is((char) eof)) {
            tokens.push_back(new Syntax(SyntaxType::END_FILE));
            break;
        } else if (oneof("\r\n")) {
            while (oneof("\r\n")); // Contract newlines.
            tokens.push_back(new Syntax(SyntaxType::END_LINE));
            newline = true;
            indent = 0;
            continue; // Tokens can't span lines.
        }

        if (newline) {
            tokens.push_back(new Indent(indent));
            newline = false;
        }

        if (string()) continue;
        if (number()) continue;
        if (oper()) continue;
        if (keywOrIdent()) continue;
    } while (1);

    return tokens;
}

void
Tokenizer::next()
{
    if (ibuf < ilast - 1) {
        ch = buf[++ibuf];
    } else if (ibuf == ilast - 1) {
        ++ibuf;
        ch = (char) stream.peek();
    } else {
        buf[ibuf++] = ch;
        stream.get();
        ch = (char) stream.peek();
    }
}

void
Tokenizer::reset()
{
    ibuf = 0;
    ilast = -1;
    ch = (char) stream.peek();
}

void
Tokenizer::push()
{
    ilast = ibuf;
}

void
Tokenizer::pop()
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

void
Tokenizer::expect(bool x)
{
    if (!x) {
        throw new SyntaxError("Expected something!");
    }
}

bool
Tokenizer::is(char c)
{
    if (ch == c) {
        next();
        return true;
    }

    return false;
}

bool
Tokenizer::isnot(char c)
{
    return ch != c;
}

bool
Tokenizer::oneof(char const * chs)
{
    if (strchr(chs, ch)) {
        next();
        return true;
    }

    return false;
}

bool
Tokenizer::is(int (*cls)(int))
{
    if (cls(ch)) {
        next();
        return true;
    }

    return false;
}

bool
Tokenizer::number()
{
    bool sign = false;
    bool point = false;
    char const * num = "0123456789";
    int start = ibuf;

    push();

    if (oneof("+-"))
        sign = true;

    if (is('.'))
        point = true;

    if (!oneof(num)) {
        pop();
        return false;
    }

    while (oneof(num));

    if (is('.')) {
        if (point) {
            throw SyntaxError("Unexpected decimal point");
        } else {
            point = true;
        }
    }

    while (oneof(num));

    char * str = strndup(&buf[start], ibuf - start);

    if (point) {
        double value = atof(str);
        auto * lit = new Liter(LiterType::DEC);
        lit->dval = value;
        tokens.push_back(lit);
    } else {
        long value = atol(str);
        auto * lit = new Liter(LiterType::INT);
        lit->ival = value;
        tokens.push_back(lit);
    }

    free(str);
    return true;
}

bool
Tokenizer::keywOrIdent()
{
    int start = ibuf;

    if (!is(isalpha) && !is('_'))
        return false;

    while (is(isalnum) || is('_'));

    std::string s((char const *) &buf[start], ibuf - start);

    for (auto pair : KEYWORDS) {
        if (s == pair.first) {
            tokens.push_back(new Keyw(pair.second));
            return true;
        }
    }

    tokens.push_back(new Ident(s));
    return true;
}

bool
Tokenizer::oper()
{
    char tok[3] = { ch, '\0', '\0' };

    if (oneof(OP_CHAR1)) {
        char b = ch;

        if (isnot(' ') && oneof(OP_CHAR2)) {
            tok[1] = b;
        }
    }

    for (auto pair : OPERATORS) {
        if (pair.first == tok) {
            tokens.push_back(new Oper(pair.second));
            return true;
        }
    }

    return false;
}

bool
Tokenizer::string()
{
    bool escape = false;
    char end;

    if      (is('\'')) end = '\'';
    else if (is('"'))  end = '"';
    else return false;

    int start = ibuf;

    do {
        if (is('\\'))
            escape = true;

        if (is(end)) {
            if (escape)
                escape = false;
            else
                break;
        }
    } while (true);

    std::string * s = new std::string(&buf[ibuf], ibuf - start - 1);
    Liter * lit = new Liter(LiterType::STR);
    lit->sval = s;
    tokens.push_back(lit);

    return true;
}

Token *
Token::parse(std::istream& stream)
{
    int c = ' ';
    char token[64];
    char * t = token;

    while (isblank(c)) {
        c = stream.get();
    }

    if (c == '#') {
        while (!iseol(c) || !iseof(c)) {
            c = stream.get();
        }
    }

    if (iseof(c)) {
        return new Syntax(SyntaxType::END_FILE);
    }

    if (iseol(c)) {
        return new Syntax(SyntaxType::END_LINE);
    }

    switch (c) {
    case '+': return new Oper(OperType::ADD);
    case '-': return new Oper(OperType::SUB);
    case '=': return new Oper(OperType::ASSIGN);
    case ':': return new Syntax(SyntaxType::COLON);
    default: break;
    }

    if (isalpha(c)) {
        *(t++) = (char) c;

        while (true) {
            c = stream.peek();
            bool keepNext = issyntax(c) || isoper(c);

            if (iseof(c) || iseol(c) || isspace(c) || keepNext) {
                // Complete the token
                if (!keepNext) {
                    stream.get();
                }

                *(t++) = '\0';
                break;
            } else if (isident(c)) {
                // The identifier continues
                *(t++) = (char) c;
                stream.get();
                continue;
            } else {
                // Invalid identifier character
                return nullptr;
            }
        }

        string id(token);

        if (id == "lambda") {
            return new Oper(OperType::LAMBDA);
        } else if (id == "True") {
            Liter * lit = new Liter(LiterType::BOOL);
            lit->bval = true;
            return lit;
        } else if (id == "False") {
            Liter * lit = new Liter(LiterType::BOOL);
            lit->bval = false;
            return lit;
        } else {
            return new Ident(move(id));
        }
    }

    if (isnumber(c)) {
        bool isDecimal = false;
        *(t++) = (char) c;

        while (true) {
            c = stream.peek();
            bool keepNext = issyntax(c) || isoper(c);

            if (iseof(c) || iseol(c) || isspace(c) || keepNext) {
                // Complete the token
                if (!keepNext) {
                    stream.get();
                }

                *(t++) = '\0';
                break;
            } else if (c == '.') {
                if (isDecimal) {
                    return nullptr;
                }

                *(t++) = (char) c;
                isDecimal = true;
                stream.get();
                continue;
            } else if (isnumber(c)) {
                // The numerical token continues
                *(t++) = (char) c;
                stream.get();
                continue;
            } else {
                // Invalid number
                return nullptr;
            }
        }

        char * discard;
        Liter * lit;

        if (isDecimal) {
            lit = new Liter(LiterType::DEC);
            lit->dval = strtod(token, &discard);
        } else {
            lit = new Liter(LiterType::INT);
            lit->ival = atol(token);
        }

        return lit;
    }

    return nullptr;
}

bool
Token::isEOL() const
{
    if (tokenType == TokenType::SYNTAX) {
        auto stx = dynamic_cast<Syntax const *>(this);
        return stx->syntaxType == SyntaxType::END_LINE;
    }

    return false;
}

bool
Token::isEOF() const
{
    if (tokenType == TokenType::SYNTAX) {
        auto stx = dynamic_cast<Syntax const *>(this);
        return stx->syntaxType == SyntaxType::END_FILE;
    }

    return false;
}

Liter::Liter(LiterType type)
: Token(TokenType::LITER), literType(type)
{
}

static string const liter_true = "1b";
static string const liter_false = "0b";
static string const liter_sq = "\"";

std::string
Liter::toString()
{
    switch (literType) {
    case LiterType::BOOL: return bval ? liter_true : liter_false;
    case LiterType::DEC: return to_string(dval) + "d";
    case LiterType::INT: return to_string(ival) + "i";
    case LiterType::STR: return liter_sq + *sval + liter_sq;
    }
}

Ident::Ident(string name)
: Token(TokenType::IDENT), name(move(name))
{
}

std::string
Ident::toString()
{
    return name + "*";
}

Oper::Oper(OperType type)
: Token(TokenType::OPER), operType(type)
{
}

std::string
Oper::toString()
{
    for (auto pair : OPERATORS) {
        if (pair.second == operType) {
            return pair.first;
        }
    }

    return "?";
}

Syntax::Syntax(SyntaxType type)
: Token(TokenType::SYNTAX), syntaxType(type)
{
}

std::string
Syntax::toString()
{
    switch (syntaxType) {
    case SyntaxType::END_LINE: return "\n";
    case SyntaxType::END_FILE: return "__eof__";
    default: break;
    }

    for (auto pair : SYNTAXES) {
        if (pair.second == syntaxType) {
            return pair.first;
        }
    }

    return "?";
}

Keyw::Keyw(KeywType type)
: Token(TokenType::KEYW), keywType(type)
{
}

std::string
Keyw::toString()
{
    for (auto pair : KEYWORDS) {
        if (pair.second == keywType) {
            return pair.first;
        }
    }

    return "?";
}

Indent::Indent(int depth)
: Token(TokenType::INDENT), depth(depth)
{
}

std::string
Indent::toString()
{
    return ">" + to_string(depth);
}
