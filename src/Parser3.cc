#include <llvmPy/Parser3.h>
#include <iostream>
using namespace llvmPy;

static std::map<TokenType, int>
initPrecedence()
{
    // Follows the table at:
    // https://docs.python.org/3/reference/expressions.html#operator-precedence
    // (where lower precedences are at the top).
    std::map<TokenType, int> map;

    map[tok_comma] = -1;

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

    map[tok_dot] = 16;

    map[tok_comma] = 17; // Tuple binding

    return map;
}

Expr *
Parser3::Expression()
{
    return Expression(0);
}

Expr *
Parser3::Expression(int mp)
{
    return Expression(mp, false);
}

Expr *
Parser3::Expression(int const mp, bool const isTupled)
{
    Expr *lhs = nullptr;

    if (!lhs) lhs = LambdaExpression(mp);
    if (!lhs) lhs = UnaryExpression(mp);
    if (!lhs) lhs = Subexpression();
    if (!lhs) lhs = Identifier();
    if (!lhs) lhs = NumericLiteral();
    if (!lhs) lhs = StringLiteral();
    assert(lhs);

    while (!EndOfExpression()) {
        TokenType const token = peek();
        int const cp = getPrecedence(token);
        int const nmp = cp + (isLeftAssociative(token) ? 1 : 0);

        if (cp >= 0 && cp < mp) {
            break;
        }

        if (token == tok_comma) {
            if (isTupled) {
                break;
            } else {
                lhs = TupleExpression(0, lhs);
                continue;
            }
        }

    }

    return lhs;
}

bool
Parser3::EndOfExpression()
{
    return is(tok_eol) || is(tok_eof) || is(tok_semicolon);
}

Expr *
Parser3::Subexpression()
{
    if (is(tok_lp)) {
        Expr *expr = Expression(0);
        assert(is(tok_rp));
        return expr;
    } else {
        return nullptr;
    }
}

BinaryExpr *
Parser3::BinaryExpression(int const mp)
{
    return nullptr;
}

LambdaExpr *
Parser3::LambdaExpression(int mp)
{
    if (is(kw_lambda)) {
        auto args = ArgumentDefinitions();
        assert(is(tok_colon));
        Expr *body = Expression(0);
        assert(body);
        auto *lambda = new LambdaExpr(*body);

        for (auto &arg : args) {
            lambda->addArgument(arg);
        }

        return lambda;
    } else {
        return nullptr;
    }
}

UnaryExpr *
Parser3::UnaryExpression(int mp)
{
    TokenType op;

    if (is(tok_add)) {
        op = tok_add;
    } else if (is(tok_sub)) {
        op = tok_sub;
    } else {
        return nullptr;
    }

    int cp = getUnaryPrecedence(op);
    int nmp = cp;

    Expr *expr = Expression(nmp);
    assert(expr);

    return new UnaryExpr(op, *expr);
}

TupleExpr *
Parser3::TupleExpression(int mp, Expr * const lhs)
{
    assert(mp == 0);
    assert(lhs);

    auto *tuple = new TupleExpr();
    tuple->addMember(*lhs);

    while (true) {
        if (is(tok_comma)) {
            if (auto *rhs = Expression(0, true)) {
                tuple->addMember(*rhs);
            } else {
                break;
            }
        } else {
            break;
        }
    }

    return tuple;
}

Stmt *
Parser3::Statement()
{
    return Statement(0);
}

/** Parse statement at the givent indent level. **/
Stmt *
Parser3::Statement(int const outer)
{
    int const indent = ExpectIndent();
    assert(indent == outer);
    if (auto *stmt = SimpleStatement()) return stmt;
    if (auto *stmt = BlockStatement(indent)) return stmt;
    return nullptr;
}

Stmt *
Parser3::SimpleStatement()
{
    if (auto *stmt = PassStatement()) return stmt;
    if (auto *stmt = BreakStatement()) return stmt;
    if (auto *stmt = ReturnStatement()) return stmt;
    return nullptr;
}

Stmt *
Parser3::BlockStatement(int indent)
{
    if (auto *stmt = WhileStatement(indent)) return stmt;
    return nullptr;
}

BreakStmt *
Parser3::BreakStatement()
{
    if (is(kw_break)) {
        return new BreakStmt();
    } else {
        return nullptr;
    }
}

ReturnStmt *
Parser3::ReturnStatement()
{
    if (is(kw_return)) {
        return new ReturnStmt(ExpectExpression());
    } else {
        return nullptr;
    }
}

WhileStmt *
Parser3::WhileStatement(int indent)
{
    if (is(kw_while)) {
        auto &cond = ExpectExpression();
        assert(is(tok_colon) && "expected ':'");
        CompoundStmt *body = nullptr;

        if (EndOfLine()) {
            body = CompoundStatement(indent);
            assert(body && "expected compound statement");
        } else {
            auto *stmt = SimpleStatement();
            assert(stmt && "expected simple statement");
            body = new CompoundStmt();
            body->addStatement(*stmt);
        }

        return new WhileStmt(cond, *body);
    } else {
        return nullptr;
    }
}

CompoundStmt *
Parser3::CompoundStatement(int outer)
{
    // The Lexer is expected to parse lines such that they always
    // have an indent token at the first position. Top-level statements
    // should have an indent of zero.

    // CompoundStatement() is always called at the beginning of a line.

    auto *result = new CompoundStmt();
    int const inner = ExpectIndent();
    assert(inner > outer);
    back();

    while (!EndOfFile()) {
        int const indent = ExpectIndent(); // Even empty lines have indents.

        if (EndOfLine()) continue; // Ignore empty lines.
        if (EndOfFile()) continue; // Ignore empty lines.

        if (indent <= outer) {
            break;
        } else {
            assert(indent == inner);
        }

        auto *statement = Statement(inner);
        assert(statement);
        result->addStatement(*statement);
    }

    // Blocks must have at least one statement (or "pass").
    assert(result->statementsCount() > 0);
    return result;
}

CompoundStmt &
Parser3::ExpectModule()
{
    // The Lexer is expected to parse lines such that they always
    // have an indent token at the first position. Top-level statements
    // should have an indent of zero.

    auto *module = CompoundStatement(0);
    return *module;

}

Expr &
Parser3::ExpectExpression()
{
    auto *expr = Expression();
    assert(expr);
    return *expr;
}

bool
Parser3::EndOfStatement()
{
    return is(tok_semicolon);
}

bool
Parser3::EndOfFile()
{
    return is(tok_eof);
}

bool
Parser3::EndOfLine()
{
    return is(tok_eol);
}
