#include <llvmPy/Parser.h>
#include <llvmPy/SyntaxError.h>
using namespace llvmPy;
using namespace std;

Parser::Parser(vector<Token> & tokens)
: tokens(tokens), lasttoken(nullptr)
{
    iter = tokens.begin();
}

bool
Parser::parse(Stmt * & out)
{
    try {
        return parse_(out);
    } catch (...) {
        return false;
    }
}

bool
Parser::parse_(Stmt * & out)
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

    if (is(kw_import)) {
        want(is(tok_ident));
        Token& module = last();
        endOfStmt();
        out = new ImportStmt(module.str);
        return true;
    }

    auto state = save();

    if (is(tok_ident)) {
        Token& tok = last();
        if (is_a(tok_assign)) {
            Expr * expr;
            if (!parse_(expr)) return false;
            out = new AssignStmt(tok.str, expr);
            return true;
        } else {
            restore(state);
        }
    }

    Expr * expr;
    if (!parse_(expr)) return false;
    endOfStmt();
    out = new ExprStmt(expr);
    return true;
}

bool
Parser::parse_(Expr * & out)
{
    Expr* lhs;
    Expr* rhs;

    if (is(tok_lp)) {
        if (!parse_(lhs))
            return false;
        want(is(tok_rp));
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
                want(is(tok_colon));
                Expr* body;
                if (!parse_(body)) return false;
                out = new LambdaExpr(body);
                return true;
            } else if (is(tok_ident)) {
                lhs = new IdentExpr(last().str);
            }
        }
    }

    if (!is_a(tok_oper)) {
        out = lhs;
        return true;
    }

    Token& op = last();
    if (!parse_(rhs)) return false;
    out = new BinaryExpr(lhs, op.type, rhs);
    return true;
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
