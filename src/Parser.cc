#include <llvmPy/Parser.h>
#include <llvmPy/SyntaxError.h>
#include <stack>
#include <llvm/Support/Casting.h>
#include <deque>

using namespace llvmPy;
using std::vector;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using llvm::dyn_cast;

static std::map<TokenType, int>
initPrecedence()
{
    // Follows the table at:
    // https://docs.python.org/3/reference/expressions.html#operator-precedence
    // (where lower precedences are at the top).
    std::map<TokenType, int> map;

    map[kw_lambda] = 1;

    map[tok_lt] = 6;
    map[tok_lte] = 6;
    map[tok_eq] = 6;
    map[tok_neq] = 6;
    map[tok_gt] = 6;
    map[tok_gte] = 6;

    map[tok_add] = 11;
    map[tok_sub] = 11;

    map[tok_mul] = 12;
    map[tok_div] = 12;

    map[tok_comma] = 17; // Tuple binding

    return map;
}

Parser::Parser(vector<Token> & tokens)
: tokens(tokens),
  precedence(initPrecedence()),
  lasttoken(nullptr)
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
        vector<string const> args;
        vector<Stmt *> stmts;

        if (!is(tok_rp)) {
            while (true) {
                want(tok_ident);
                args.push_back(*last().str);
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
            int stmtDepth = last().depth;
            if (stmtDepth < innerIndent) break;
            if (stmtDepth > innerIndent) {
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

std::unique_ptr<Expr>
Parser::parseExpr2()
{
    // Implements the "train-yard shunting algorithm" for operator precedence.
    std::deque<unique_ptr<Expr>> output;
    std::stack<unique_ptr<TokenExpr>> operators;

    while (true) {

        if (auto *expr = parseLitExpr()) {
            output.emplace_front(expr);
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
            output.emplace_front(new LambdaExpr(args, body));
        } else if (is(tok_ident)) {
            output.emplace_front(new IdentExpr(last().str));
        } else if (is(tok_lp)) {
            operators.emplace(new TokenExpr(tok_lp));
        } else if (is(tok_comma)) {
            // Create or extend a tuple. This is among the
            // most binding operators.
            output.emplace_front(new TokenExpr(tok_comma));
        } else if (is(tok_rp)) {
            for (;;) {
                auto expr = std::move(operators.top());
                operators.pop();

                // Pop operators until the first '(' is encountered.
                if (auto *token = llvm::dyn_cast<TokenExpr>(expr.get())) {
                    if (token->getTokenType() == tok_lp) {
                        break;
                    }
                }

                output.push_front(std::move(expr));
            }

            // Mark end of group (i.e. call).
            output.emplace_front(new TokenExpr(tok_rp));
        } else if (is_a(tok_oper)) {
            if (operators.empty()) {
                operators.emplace(new TokenExpr(last().type));
            } else {
                for (;;) {
                    int tokenPrecedence = getPrecedence(last().type);
                    int topPrecedence = getPrecedence(
                            operators.top()->getTokenType());

                    if (tokenPrecedence <= topPrecedence) {
                        auto top = std::move(operators.top());
                        operators.pop();
                        output.push_front(std::move(top));
                    } else {
                        operators.emplace(new TokenExpr(last().type));
                        break;
                    }
                }
            }
        }

        break;
    }

    // Pop remaining stack to output.
    while (!operators.empty()) {
        auto op = std::move(operators.top());
        operators.pop();
        output.push_front(std::move(op));
    }

    // Assemble AST by popping output in reverse.
    std::stack<std::unique_ptr<Expr>> result;
    while (!output.empty()) {
        auto expr = std::move(output.back());
        output.pop_back();

        if (auto *e = dyn_cast<LitExpr>(expr.get())) {
            result.push(std::move(expr));
        } else if (auto *e = dyn_cast<TokenExpr>(expr.get())) {
            auto token = e->getTokenType();

            if (token == tok_comma) {
                auto right = std::move(result.top());
                result.pop();
                auto left = std::move(result.top());
                result.pop();

                if (auto *t = dyn_cast<TupleExpr>(right.get())) {
                    // TODO: Should be reverse.
                    t->addMember(std::move(left));
                    result.push(std::move(right));
                } else {
                    auto tuple = make_unique<TupleExpr>();
                    // TODO: Should be reverse.
                    t->addMember(std::move(right));
                    t->addMember(std::move(left));
                    result.push(std::move(tuple));
                }
            }
        }
    }

    // TODO: This is a placeholder.
    return std::move(output.front());
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
        vector<Expr *> args;

        if (!is(tok_rp)) {
            while (true) {
                args.push_back(parseExpr());
                if (is(tok_rp)) break;
                else want(tok_comma);
            }
        }

        auto *call = new CallExpr(unique_ptr<Expr>(lhs));
        lhs = call;

        for (auto &arg : args) {
            auto ptr = unique_ptr<Expr>(arg);
            call->addArgument(std::move(ptr));
        }
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
        auto value = std::make_unique<std::string const>(
                tok.str->substr(1, tok.str->length() - 1));
        return new StrLitExpr(std::move(value));
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
