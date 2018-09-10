#include <llvmPy/AST/Token.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
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
    return name;
}

Oper::Oper(OperType type)
: Token(TokenType::OPER), operType(type)
{
}

static string const oper_assign = "=";
static string const oper_lambda = "lambda ";
static string const oper_add = "+";
static string const oper_sub = "-";

std::string
Oper::toString()
{
    switch (operType) {
    case OperType::ASSIGN: return oper_assign;
    case OperType::LAMBDA: return oper_lambda;
    case OperType::ADD: return oper_add;
    case OperType::SUB: return oper_sub;
    }
}

Syntax::Syntax(SyntaxType type)
: Token(TokenType::SYNTAX), syntaxType(type)
{
}

static string const syntax_colon = ":";
static string const syntax_lparen = "(";
static string const syntax_rparen = ")";

std::string
Syntax::toString()
{
    switch (syntaxType) {
    case SyntaxType::COLON: return syntax_colon;
    case SyntaxType::L_PAREN: return syntax_lparen;
    case SyntaxType::R_PAREN: return syntax_rparen;

    default:
        return "";
    }
}
