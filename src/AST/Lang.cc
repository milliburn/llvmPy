#include <llvmPy/AST/Lang.h>
using namespace llvmPy::AST;
using namespace std;

array<pair<string const, OperType> const, OPERATORS_COUNT> const
llvmPy::AST::OPERATORS = {
        make_pair("+", OperType::ADD),
        make_pair("-", OperType::SUB),
        make_pair("*", OperType::MUL),
        make_pair("/", OperType::DIV),
        make_pair("!", OperType::NOT),
        make_pair("==", OperType::EQUALS),
};

array<pair<string const, KeywType> const, KEYWORDS_COUNT> const
llvmPy::AST::KEYWORDS = {
        make_pair("lambda", KeywType::LAMBDA),
        make_pair("pass", KeywType::PASS),
        make_pair("def", KeywType::DEFUN),
        make_pair("import", KeywType::IMPORT),
        make_pair("from", KeywType::FROM),
        make_pair("if", KeywType::IF),
        make_pair("else", KeywType::ELSE),
        make_pair("elif", KeywType::ELIF),
};

array<pair<string const, SyntaxType> const, SYNTAXES_COUNT> const
llvmPy::AST::SYNTAXES = {
        make_pair("(", SyntaxType::L_PAREN),
        make_pair(")", SyntaxType::R_PAREN),
        make_pair(":", SyntaxType::COLON),
        make_pair("=", SyntaxType::ASSIGN),
        make_pair(";", SyntaxType::SEMICOLON),
        make_pair(".", SyntaxType::DOT),
};

