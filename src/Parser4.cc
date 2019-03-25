#include <llvmPy/Parser4.h>
#include <assert.h>
#include <stdexcept>
using namespace llvmPy;

static std::unordered_map<TokenType, int>
generatePrecedenceMap()
{
    // Follows the table at:
    // https://docs.python.org/3/reference/expressions.html#operator-precedence
    // (where lower precedences are at the top).
    std::unordered_map<TokenType, int> map;
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
    map[tok_lp] = 17;
    map[tok_rp] = 17;
    return map;
}

Parser4::Parser4(std::vector<Token> const &tokens)
    : _tokens(tokens.data()),
      _tokenCount(tokens.size()),
      _index(0),
      _precedence(generatePrecedenceMap())
{
}

bool
Parser4::EndOfFile()
{
    return is(tok_eof);
}

bool
Parser4::EndOfLine()
{
    return is(tok_eol);
}

bool
Parser4::EmptyLine()
{
    if (Indentation() >= 0) {
        if (EndOfLine() || EndOfFile()) {
            return true;
        } else {
            back();
            return false;
        }
    } else {
        return false;
    }
}

int
Parser4::Indentation()
{
    if (peek(tok_indent)) {
        auto &indent = take();
        return static_cast<int>(indent.getDepth());
    } else {
        return -1;
    }
}

TokenType
Parser4::UnaryOperator()
{
    auto &token = peek();
    auto type = token.getTokenType();

    if (type & tok_unary) {
        next();
        assert(_precedence.count(type));
        return type;
    } else {
        return tok_null;
    }
}

TokenType
Parser4::BinaryOperator()
{
    auto &token = peek();
    auto type = token.getTokenType();

    if (type & tok_binary) {
        next();
        assert(_precedence.count(type));
        return type;
    } else {
        return tok_null;
    }
}

std::vector<std::string>
Parser4::FunctionArguments()
{
    // TODO: Add support for *args, **kwargs.
    std::vector<std::string> result;

    do {
        if (auto *expr = Identifier()) {
            result.push_back(expr->getName());
        } else {
            break;
        }
    } while (is(tok_comma));

    return result;
}

Expr *
Parser4::Expression()
{
    Expr *result = ValueExpression(0);
    TupleExpr *tuple = nullptr;

    if (!result) {
        return nullptr;
    }

    while (is(tok_comma)) {
        if (!tuple) {
            tuple = new TupleExpr();
            tuple->addMember(*result);
            result = tuple;
        }

        if (auto *expr = ValueExpression(0)) {
            tuple->addMember(*expr);
        } else {
            break;
        }
    }

    return result;
}

Expr *
Parser4::ValueExpression(int minimumPrecedence)
{
    Expr *result = nullptr;

    if (EndOfLine()) return nullptr;
    if (EndOfFile()) return nullptr;
    if (!result) result = Identifier();
    if (!result) result = NumericLiteral();
    if (!result) result = StringLiteral();
    if (!result) result = UnaryExpression();
    if (!result) result = Subexpression();
    if (!result) result = LambdaExpression();
    if (!result) return nullptr;

    while (!EndOfLine() && !EndOfFile()) {
        if (auto *expr = BinaryExpression(minimumPrecedence, *result)) {
            result = expr;
        } else {
            break;
        }
    }

    return result;
}

Expr *
Parser4::Subexpression()
{
    if (is(tok_lp)) {
        Expr *result = Expression();
        syntax(is(tok_rp), "Expected ')'");
        if (!result) result = new TupleExpr();
        return result;
    } else {
        return nullptr;
    }
}

Expr *
Parser4::UnaryExpression()
{
    if (auto operator_ = UnaryOperator()) {
        auto *expr = Expression();
        syntax(expr, "Expected unary expression");
        return new UnaryExpr(operator_, *expr);
    } else {
        return nullptr;
    }
}

Expr *
Parser4::BinaryExpression(int minimumPrecedence, Expr &lhs)
{
    if (auto operator_ = BinaryOperator()) {
        int operatorPrecedence = precedence(operator_);

        if (operatorPrecedence < minimumPrecedence) {
            back();
            return nullptr;
        }

        bool isLeftAssoc = isLeftAssociative(operator_);
        int nextPrecedence = operatorPrecedence + (isLeftAssoc ? 1 : 0);

        auto *rhs = ValueExpression(nextPrecedence);
        syntax(rhs, "Expected right-hand side of binary expression");
        return new BinaryExpr(lhs, operator_, *rhs);
    } else {
        return nullptr;
    }
}

Expr *
Parser4::LambdaExpression()
{
    if (is(kw_lambda)) {
        auto arguments = FunctionArguments();
        syntax(is(tok_colon), "Expected ':'");
        auto *body = ValueExpression(0);
        syntax(body, "Expected value expression in lambda function");
        auto *lambda = new LambdaExpr(*body);
        for (auto &arg : arguments) {
            lambda->addArgument(arg);
        }
        return lambda;
    } else {
        return nullptr;
    }
}

IdentExpr *
Parser4::Identifier()
{
    if (peek(tok_ident)) {
        auto &token = take();
        return new IdentExpr(token.getString());
    } else {
        return nullptr;
    }
}

Expr *
Parser4::NumericLiteral()
{
    if (peek(tok_number)) {
        auto &text = take().getString();

        if (text.find('.') != std::string::npos) {
            double value = atof(text.c_str());
            return new DecimalExpr(value);
        } else {
            int64_t value = atol(text.c_str());
            return new IntegerExpr(value);
        }
    } else {
        return nullptr;
    }
}

Expr *
Parser4::StringLiteral()
{
    if (peek(tok_string)) {
        auto &text = take().getString();
        // Remove " or ' delimiters.
        auto value = text.substr(1, text.length() - 2);
        return new StringExpr(value);
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::Statement()
{
    return Statement(0);
}

Stmt *
Parser4::Statement(int const outerIndent)
{
    Stmt *result = nullptr;
    int const indent = Indentation();
    syntax(indent >= 0, "Expected indentation");
    syntax(indent == outerIndent, "Expected statement indentation");
    if (!result) result = SimpleStatement();
    if (!result) result = BlockStatement(indent);
    return result;
}

Stmt *
Parser4::SimpleStatement()
{
    Stmt *result = nullptr;
    if (!result) result = BreakStatement();
    if (!result) result = ContinueStatement();
    if (!result) result = PassStatement();
    if (!result) result = ReturnStatement();
    if (!result) result = AssignStatement();
    if (!result) result = ExpressionStatement(); // Must be last.
    return result;
}

Stmt *
Parser4::BreakStatement()
{
    if (is(kw_break)) {
        return new BreakStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::ContinueStatement()
{
    if (is(kw_continue)) {
        return new ContinueStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::PassStatement()
{
    if (is(kw_pass)) {
        return new PassStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::ExpressionStatement()
{
    if (auto *expr = Expression()) {
        return new ExprStmt(*expr);
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::ReturnStatement()
{
    if (is(kw_return)) {
        auto *expr = Expression();
        syntax(expr, "Expected expression");
        return new ReturnStmt(*expr);
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::AssignStatement()
{
    if (auto *ident = Identifier()) {
        if (is(tok_assign)) {
            auto *expr = Expression();
            syntax(expr, "Expected expression");
            return new AssignStmt(ident->getName(), *expr);
        } else {
            back();
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::BlockStatement(int outerIndent)
{
    Stmt *result = nullptr;
    if (!result) result = IfStatement(outerIndent);
    if (!result) result = WhileStatement(outerIndent);
    return result;
}

CompoundStmt *
Parser4::CompoundStatement(int outerIndent)
{
    auto *result = new CompoundStmt();
    int const innerIndent = Indentation();
    syntax(innerIndent >= 0, "Expected indentation");
    syntax(innerIndent > outerIndent, "Expected block statement indentation");
    back();

    while (!EndOfFile()) {
        int const indent = Indentation();
        syntax(indent >= 0, "Expected indentation");

        if (EndOfLine()) continue; // Ignore empty lines.
        if (EndOfFile()) continue; // Ignore empty lines.
        back();

        if (indent <= outerIndent) {
            break;
        } else {
            syntax(indent == innerIndent, "Expected block indentation");
        }

        auto *statement = Statement(innerIndent);
        syntax(statement, "Expected statement");
        result->addStatement(*statement);
    }

    syntax(result->statementsCount() > 0, "Expected block statement");
    return result;
}

Stmt *
Parser4::WhileStatement(int outerIndent)
{
    if (is(kw_while)) {
        auto *condition = Expression();
        syntax(condition, "Expected 'while' condition expression");
        syntax(is(tok_colon), "Expected ':'");
        syntax(EndOfLine(), "Expected end of line");
        auto *body = CompoundStatement(outerIndent);
        syntax(body, "Expected 'while' statement body");
        return new WhileStmt(*condition, *body);
    } else {
        return nullptr;
    }
}

Stmt *
Parser4::IfStatement(int outerIndent)
{
    return IfStatement(outerIndent, false);
}

Stmt *
Parser4::IfStatement(int outerIndent, bool isElif)
{
    if (is(isElif ? kw_elif : kw_if)) {
        auto *condition = Expression();
        syntax(condition, "Expected 'if' condition expression");
        syntax(is(tok_colon), "Expected ':'");
        syntax(EndOfLine(), "Expected end of line");
        CompoundStmt *thenBranch = CompoundStatement(outerIndent);
        syntax(thenBranch, "Expected 'if' statement body");
        while (EmptyLine()); // TODO: These should be skipped by the tokenizer.

        CompoundStmt *elseBranch = nullptr;
        int const indent = Indentation();

        if (indent == outerIndent && is(kw_else)) {
            syntax(is(tok_colon), "Expected ':'");
            syntax(EndOfLine(), "Expected end of line");
            elseBranch = CompoundStatement(outerIndent);
        } else if (indent == outerIndent && peek(kw_elif)) {
            auto *elifBranch = IfStatement(outerIndent, true);
            syntax(elifBranch, "Expected 'elif' branch");
            elseBranch = new CompoundStmt(*elifBranch);
        } else {
            elseBranch = new CompoundStmt(*new PassStmt());
        }

        assert(elseBranch);
        return new ConditionalStmt(*condition, *thenBranch, *elseBranch);
    } else {
        return nullptr;
    }
}

void
Parser4::next()
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    if (_index < _tokenCount - 1) {
        // Stop advancing once EOF is reached.
        _index++;
    }
}

void
Parser4::back()
{
    assert(_index > 0);
    assert(_index < _tokenCount);
    _index--;
}

bool
Parser4::is(TokenType const type)
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    auto &token = peek();

    if (token.getTokenType() == type) {
        next();
        return true;
    } else {
        return false;
    }
}

Token const &
Parser4::peek() const
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    auto &token = _tokens[_index];

    if (_index == _tokenCount - 1) {
        assert(token.getTokenType() == tok_eof);
    }

    return token;
}

bool
Parser4::peek(TokenType type) const
{
    return peek().getTokenType() == type;
}

Token const &
Parser4::take()
{
    auto &token = peek();
    next();
    return token;
}

int
Parser4::precedence(Token const &token) const
{
    return precedence(token.getTokenType());
}

int
Parser4::precedence(TokenType type) const
{
    assert(_precedence.count(type));
    return _precedence.at(type);
}

bool
Parser4::isLeftAssociative(Token const &token) const
{
    return isLeftAssociative(token.getTokenType());
}

bool
Parser4::isLeftAssociative(TokenType type) const
{
    return ! (type & tok_rassoc);
}

void
Parser4::syntax(bool condition, std::string const &message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}
