#include <llvmPy/Parser.h>
#include <llvmPy/SyntaxError.h>
using namespace llvmPy::AST;
using namespace std;

Parser::Parser(vector<Token> & tokens)
: tokens(tokens), lasttoken((TokenType) 0)
{
    iter = tokens.begin();
}

bool
Parser::parse(Stmt & out)
{
    try {
        return parseStmt(out);
    } catch (...) {
        return false;
    }
}

bool
Parser::parseStmt(Stmt & out)
{
    /*
    if (is(tok_ident, "def")) {
        // TODO: Read indent.
        want(is(tok_lp));
        want(is(tok_rp));
        want(is(tok_colon));
        want(is(tok_eol));
        // TODO: Require deeper indent.
        do stmt(); while (is(tok_indent));
        return true;
    } else */

    is(tok_indent); // Ignore indentation for now.

    if (is(tok_ident, "import")) {
        out.type = stmt_import;
        want(is(tok_ident));
        out.import.module = last();
        endOfStmt();
        return true;
    }

    auto state = save();

    if (is(tok_ident)) {
        Token & tok = last();
        if (is_a(tok_assign)) {
            out.type = stmt_assign;
            return parseExpr(out.expr);
        } else {
            restore(state);
        }
    }

    out.type = stmt_expr;
    if (!parseExpr(out.expr)) return false;
    endOfStmt();
    return true;
}

bool
Parser::parseExpr(Expr & out)
{
    Expr lhs, rhs;

    if (is(tok_lp)) {
        if (!parseExpr(lhs))
            return false;
        want(is(tok_rp));
    } else if (is(tok_string)) {
        Token tok = last();
        lhs.type = expr_strlit;
        lhs.strlit = tok.str.substr(1, tok.str.length() - 1);
    } else if (is(tok_number)) {
         // TODO: Negative, positive numbers.
        Token tok = last();
        lhs.type = expr_numlit;

        if (tok.str.find('.') != string::npos) {
            lhs.numlit.type = num_double;
            lhs.numlit.dval = atof(tok.str.c_str());
        } else {
            lhs.numlit.type = num_int;
            lhs.numlit.ival = atol(tok.str.c_str());
        }
    } else if (is(tok_ident, "lambda")) {
        want(is(tok_colon));
        return parseExpr(out);
    } else if (is(tok_ident), "True") {
        lhs.type = expr_boollit;
        lhs.bval = true;
    } else if (is(tok_ident), "False") {
        lhs.type = expr_boollit;
        lhs.bval = false;
    } else if (is(tok_ident)) {
        lhs.type = expr_ident;
        lhs.ident = last();
    }

    out = lhs;

    return true;
}

void
Parser::next()
{
    lasttoken = *iter;
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
    return lasttoken;
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

bool
Parser::is(TokenType type, std::string const & str)
{
    if ((*iter).type == type && (*iter).str == str) {
        next();
        return true;
    } else {
        return false;
    }
}

void
Parser::want(bool cond)
{
    if (!cond) {
        throw SyntaxError("Wanted something!");
    }
}

void
Parser::endOfStmt()
{
    if (!is(tok_semicolon) && !is(tok_eol) && !is(tok_eof))
        throw SyntaxError("Expected end of statement");
}
