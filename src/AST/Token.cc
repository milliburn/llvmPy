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
issyntax(int c) {
    return strchr("()=", c) || iseof(c);
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

Token::~Token()
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

    if (iseof(c) || isspace(c)) {
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

            if (iseof(c) || isspace(c) || keepNext) {
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

            if (iseof(c) || isspace(c) || keepNext) {
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

Liter::Liter(LiterType type)
: Token(TokenType::LITER), literType(type)
{
}

Ident::Ident(string name)
: Token(TokenType::IDENT), name(move(name))
{
}

Oper::Oper(OperType type)
: Token(TokenType::OPER), operType(type)
{
}

Syntax::Syntax(SyntaxType type)
: Token(TokenType::SYNTAX), syntaxType(type)
{
}
