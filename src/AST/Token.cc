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

Token::Token(TokenType tokenType)
: tokenType(tokenType)
{
}

TokenType
Token::getType()
{
    return tokenType;
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
        return new EOL();
    }

    if (strchr("+-", c)) {
        return new Oper(c);
    }

    if (isnumber(c)) {
        bool isDecimal = false;
        bool isComplete = false;
        *(t++) = (char) c;

        while (!isComplete) {
            c = stream.peek();
            bool keepNext = strchr("+-()", c);

            if (iseof(c) || isspace(c) || keepNext) {
                // Complete the token
                if (!keepNext) {
                    stream.get();
                }

                *(t++) = '\0';
                isComplete = true;
            } else if (c == '.') {
                if (isDecimal) {
                    return nullptr;
                }

                *(t++) = (char) c;
                isDecimal = true;
            } else if (isnumber(c)) {
                // The numerical token continues
                *(t++) = (char) c;
            } else {
                // Invalid number
                return nullptr;
            }
        }

        char * discard;
        double datum = std::strtod(token, &discard);
        return new NumLit(datum);
    }

    return nullptr;
}

std::string
Token::toString()
{
    return "???";
}

NumLit::NumLit(double datum)
: Token(TokenType::NUM_LIT)
{
    this->datum = datum;
}

std::string
NumLit::toString()
{
    return to_string(datum);
}

Oper::Oper(int datum)
: Token(TokenType::OPER)
{
    this->datum = datum;
}

std::string
Oper::toString()
{
    return to_string((char) datum);
}

EOL::EOL()
: Token(TokenType::EOL)
{
}

std::string
EOL::toString()
{
    return "EOL";
}
