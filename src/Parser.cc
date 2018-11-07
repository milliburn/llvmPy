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

    long indent = 0;

    if (is(tok_indent)) {
        indent = last().depth;
    }

    if (is(kw_import)) {
        want(tok_ident);
        Token &module = last();
        parseEndOfStmt();
        return new ImportStmt(module.str);
    } else if (is(kw_def)) {
        want(tok_ident);
        Token &ident = last();
        want(tok_lp);
        vector<string const *> args;
        vector<Stmt *> stmts;

        if (!is(tok_rp)) {
            while (true) {
                want(tok_ident);
                args.push_back(last().str);
                if (is(tok_rp)) break;
                else want(tok_comma);
            }
        }

        want(tok_colon);
        want(tok_eol);

        want(tok_indent);
        long innerIndent = last().depth;

        if (innerIndent <= indent) {
            throw SyntaxError("Invalid indent");
        }

        stmts.push_back(parseStmt());

        while (true) {
            if (is(tok_eof)) break;
            want(tok_indent);
            if (last().depth < indent) break;
            if (last().depth > innerIndent) {
                throw SyntaxError("Invalid indent");
            }
            stmts.push_back(parseStmt());
        }

        return new DefStmt(*ident.str, args, stmts);
    } else if (is(kw_return)) {
        auto &expr = *parseExpr();
        parseEndOfStmt();
        return new ReturnStmt(expr);
    }

    auto state = save();

    if (is(tok_ident)) {
        Token& tok = last();
        if (is_a(tok_assign)) {
            Expr * expr = parseExpr();
            parseEndOfStmt();
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

    // Parse left-hand-side expression.

    if ((lhs = parseLitExpr())) {
        // Ignore.
    } else if (is(tok_lp)) {
        lhs = parseExpr();
        want(tok_rp);
    } else if (is(kw_lambda)) {
        vector<string const> args;

        while (true) {
            if (is(tok_ident)) {
                Token &ident = last();
                std::string str = *ident.str;
                args.push_back(str);
                if (is(tok_comma)) continue;
            }

            want(tok_colon);
            break;
        }

        Expr *body = parseExpr();
        return new LambdaExpr(args, body);
    } else if (is(tok_ident)) {
        lhs = new IdentExpr(last().str);
    } else {
        throw ParserError("Unknown situation");
    }

    // Extended parse of left-hand-side expression.

    if (is(tok_lp)) {
        vector<Expr const *> args;

        if (!is(tok_rp)) {
            while (true) {
                args.push_back(parseExpr());
                if (is(tok_rp)) break;
                else want(tok_comma);
            }
        }

        lhs = new CallExpr(lhs, args);
    }

    // Parse right-hand-side expression.

    if (is_a(tok_oper)) {
        Token &op = last();
        rhs = parseExpr();
        return new BinaryExpr(lhs, op.type, rhs);
    } else {
        return lhs;
    }
}

LitExpr *
Parser::parseLitExpr()
{
    if (is(tok_string)) {
        Token tok = last();
        return new StrLitExpr(tok.str->substr(1, tok.str->length() - 1));
    }

    int sign = 0;
    auto state = save();

    if (is(tok_add) || is(tok_sub)) {
        sign = last().type == tok_sub ? -1 : 1;
    }

    if (is(tok_number)) {
        Token num = last();
        if (num.str->find('.') != string::npos) {
            double val = atof(num.str->c_str());
            return new DecLitExpr(sign < 0 ? -val : val);
        } else {
            long val = atol(num.str->c_str());
            return new IntLitExpr(sign < 0 ? -val : val);
        }
    } else if (sign) {
        // Un-consume the sign (likely an operator instead).
        restore(state);
    }

    return nullptr;
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

TokenType
Parser::current()
{
    if (iter != tokens.end()) {
        return (*iter).type;
    } else {
        return tok_eof;
    }
}

bool
Parser::is(TokenType type)
{
    if (current() == type) {
        next();
        return true;
    } else {
        return false;
    }
}

bool
Parser::is_a(TokenType type)
{
    if (current() & type) {
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
