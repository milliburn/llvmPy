#include <llvmPy/Parser.h>
#include <llvmPy/SyntaxError.h>
using namespace llvmPy;
using std::vector;
using std::string;
using std::to_string;

Parser::Parser(vector<Token> & tokens)
: tokens(tokens), lasttoken(nullptr)
{
    iter = tokens.begin();
}

void
Parser::parse(vector<Stmt *> &stmts)
{
    while (true) {
        Stmt *stmt = parseStmt();
        if (!stmt) break;
        stmts.push_back(stmt);
    }
}

Stmt *
Parser::parseStmt()
{
    while (is(tok_eol));

    if (is(tok_eof)) {
        return nullptr;
    }

    is(tok_indent); // Ignore indentation for now.

    if (is(kw_import)) {
        want(tok_ident);
        Token& module = last();
        parseEndOfStmt();
        return new ImportStmt(module.str);
    }

    auto state = save();

    if (is(tok_ident)) {
        Token& tok = last();
        if (is_a(tok_assign)) {
            Expr * expr = parseExpr();
            return new AssignStmt(tok.str, expr);
        } else {
            restore(state);
        }
    }

    Expr * expr = parseExpr();
    parseEndOfStmt();
    return new ExprStmt(expr);
}

Expr *
Parser::parseExpr()
{
    Expr *lhs = nullptr;
    Expr *rhs = nullptr;

    if (is(tok_lp)) {
        lhs = parseExpr();
        want(tok_rp);
    } else if (is(tok_string)) {
        Token tok = last();
        lhs = new StrLitExpr(tok.str->substr(1, tok.str->length() - 1));
    } else {
        bool neg = false;
        auto state = save();

        if (is(tok_add) || is(tok_sub)) {
            Token s = last();
            neg = (s.type == tok_sub);
        }

        if (is(tok_number)) {
            Token num = last();
            if (num.str->find('.') != string::npos) {
                double val = atof(num.str->c_str());
                lhs = new DecLitExpr(neg ? -val : val);
            } else {
                long val = atol(num.str->c_str());
                lhs = new IntLitExpr(neg ? -val : val);
            }
        } else {
            restore(state);

            if (is(kw_lambda)) {
                want(tok_colon);
                Expr *body = parseExpr();
                return new LambdaExpr(body);
            } else if (is(tok_ident)) {
                lhs = new IdentExpr(last().str);
            }
        }
    }

    if (!is_a(tok_oper)) {
        return lhs;
    }

    Token &op = last();
    rhs = parseExpr();
    return new BinaryExpr(lhs, op.type, rhs);
}

void
Parser::next()
{
    lasttoken = &(*iter);
    iter++;
}

std::vector<Token>::iterator
Parser::save()
{
    return iter;
}

void
Parser::restore(std::vector<Token>::iterator iter)
{
    this->iter = iter;
}

Token &
Parser::last()
{
    return *lasttoken;
}

bool
Parser::is(TokenType type)
{
    if ((*iter).type == type) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Parser::is_a(TokenType type)
{
    if ((*iter).type & type) {
        next();
        return true;
    } else {
        return false;
    }
}

void
Parser::want(TokenType type)
{
    if (!is(type)) {
        throw ParserError("Expected " + to_string((int) type));
    }
}

void
Parser::want_a(TokenType type)
{
    if (!is_a(type)) {
        throw ParserError("Expected a " + to_string((int) type));
    }
}

void
Parser::parseEndOfStmt()
{
    if (!is(tok_semicolon) && !is(tok_eol) && !is(tok_eof)) {
        throw ParserError("Expected end of statement");
    }
}
