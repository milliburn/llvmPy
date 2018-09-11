#include <llvmPy/AST/Parser.h>
#include <llvmPy/AST/SyntaxError.h>
#include <llvmPy/AST/Token.h>
using namespace llvmPy::AST;

Parser::Parser(std::istream & stream)
: stream(stream)
{
}

bool
Parser::tokenize()
{
    Token * tok;

    while (true) {
        tok = Token::parse(stream);

        if (tok == nullptr) {
            return false;
        }

        tokens.push_back(tok);

        if (tok->isEOF()) {
            break;
        }
    }

    iter = tokens.begin();
    return true;
}

Stmt *
Parser::stmt()
{
    Token * token = *iter;

    next();
    push();

    if (is(KeywType::PASS)) {
        endOfStmt();
        return new PassStmt();
    } else if (is(KeywType::DEFUN)) {
        expect(TokenType::INDENT);
        expect(SyntaxType::L_PAREN);
        expect(SyntaxType::R_PAREN);
        expect(SyntaxType::COLON);
        expect(SyntaxType::END_LINE);
        expect(TokenType::INDENT);
        do stmt(); while (is(TokenType::INDENT));
        return nullptr;
    } else if (is(TokenType::IDENT)) {
        IdentExpr * lhs = new IdentExpr(*dynamic_cast<Ident *>(token));

        if (is(SyntaxType::ASSIGN)) {
            Expr * rhs = expr();
            endOfStmt();
            return new AssignStmt(*lhs, *rhs);
        } else {
            pop();
        }
    } else if (is(KeywType::IMPORT)) {
        expect(TokenType::IDENT);
        if (is(KeywType::FROM)) {
            expect(TokenType::IDENT);
        }
        endOfStmt();
        return nullptr;
    } else if (is(KeywType::FROM)) {
        expect(TokenType::IDENT);
        expect(KeywType::IMPORT);
        expect(TokenType::IDENT);
        endOfStmt();
        return nullptr;
    }

    expr();
    endOfStmt();
    return nullptr;
}

Expr *
Parser::expr()
{
    Token * token = *iter;
    Expr * lhs = nullptr;
    Expr * rhs = nullptr;

    if (is(SyntaxType::L_PAREN)) {
        lhs = expr();
        expect(SyntaxType::R_PAREN);
    } else if (is(TokenType::IDENT)) {
        lhs = new IdentExpr(*dynamic_cast<Ident *>(token));
    } else if (is(TokenType::LITER)) {
        lhs = new ConstExpr(*dynamic_cast<Liter *>(token));
    } else if (is(KeywType::LAMBDA)) {
        expect(SyntaxType::COLON);
        lhs = expr();
        return new LambdaExpr(*lhs);
    } else {
        throw SyntaxError("Invalid expression");
    }

    token = *iter;

    if (is(TokenType::OPER)) {
        rhs = expr();
        return new BinaryExpr(*lhs, *dynamic_cast<Oper *>(token), *rhs);
    } else {
        return lhs;
    }
}

void
Parser::next()
{
    iter++;
}

void
Parser::push()
{
    memory.push(iter);
}

void
Parser::pop()
{
    iter = memory.top();
    memory.pop();
}

void
Parser::discard()
{
    memory.pop();
}

bool
Parser::is(TokenType type)
{
    if ((*iter)->tokenType == type) {
        next();
        return true;
    }

    return false;
}

bool
Parser::is(SyntaxType type)
{
    if ((*iter)->tokenType == TokenType::SYNTAX) {
        if (dynamic_cast<Syntax *>(*iter)->syntaxType == type) {
            next();
            return true;
        }
    }

    return false;
}

bool
Parser::is(KeywType type)
{
    if ((*iter)->tokenType == TokenType::SYNTAX) {
        if (dynamic_cast<Keyw *>(*iter)->keywType == type) {
            next();
            return true;
        }
    }

    return false;
}

void
Parser::expect(TokenType type)
{
    if (!is(type)) {
        throw new SyntaxError("Expected TokenType::");
    }
}

void
Parser::expect(SyntaxType type)
{
    if (!is(type)) {
        throw new SyntaxError("Expected SyntaxType::");
    }
}

void
Parser::expect(KeywType type)
{
    if (!is(type)) {
        throw new SyntaxError("Expected KeywType::");
    }
}

void
Parser::endOfStmt()
{
    if (!is(SyntaxType::SEMICOLON)) {
        if (!is(SyntaxType::END_LINE)) {
            expect(SyntaxType::END_FILE);
        }
    }

    throw new SyntaxError("Unexpected end of statement");
}
