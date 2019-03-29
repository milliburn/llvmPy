#include <llvmPy/Parser.h>
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

Parser::Parser(std::vector<Token> const &tokens)
    : _tokens(tokens.data()),
      _tokenCount(tokens.size()),
      _index(0),
      _precedence(generatePrecedenceMap())
{
}

bool
Parser::EndOfFile()
{
    return is(tok_eof);
}

bool
Parser::EndOfLine()
{
    return is(tok_eol);
}

int
Parser::NextIndentation()
{
    int indent = Indentation();
    syntax(indent >= 0, "Expected indentation");
    back();
    return indent;
}

int
Parser::Indentation()
{
    if (peek(tok_indent)) {
        auto &indent = take();
        return static_cast<int>(indent.getDepth());
    } else {
        return -1;
    }
}

TokenType
Parser::UnaryOperator()
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
Parser::BinaryOperator()
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
Parser::FunctionArguments()
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
Parser::Expression()
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
Parser::ValueExpression(int minimumPrecedence)
{
    Expr *result = nullptr;
    if (!result) result = Identifier();
    if (!result) result = NumericLiteral();
    if (!result) result = StringLiteral();
    if (!result) result = UnaryExpression();
    if (!result) result = Subexpression();
    if (!result) result = LambdaExpression();
    if (!result) return nullptr;

    while (true) {
        Expr *rhs = nullptr;
        if (!rhs) rhs = CallExpression(*result);
        if (!rhs) rhs = GetattrExpression(*result);
        if (!rhs) rhs = GetitemExpression(*result);
        if (!rhs) rhs = BinaryExpression(minimumPrecedence, *result);
        if (!rhs) break;
        result = rhs;
    }

    return result;
}

Expr *
Parser::Subexpression()
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
Parser::UnaryExpression()
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
Parser::BinaryExpression(int minimumPrecedence, Expr &lhs)
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
Parser::LambdaExpression()
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

Expr *
Parser::CallExpression(Expr &callee)
{
    if (is(tok_lp)) {
        auto *call = new CallExpr(callee);

        do {
            if (peek(tok_rp)) {
                break;
            } else if (auto *value = ValueExpression(0)) {
                call->addArgument(*value);
            } else {
                syntax(false, "Expected value expression");
            }
        } while (is(tok_comma));

        syntax(is(tok_rp), "Expected ')'");

        return call;
    } else {
        return nullptr;
    }
}

Expr *
Parser::GetattrExpression(Expr &object)
{
    if (is(tok_dot)) {
        auto *ident = Identifier();
        syntax(ident, "Expected identifier");
        auto *result = new GetattrExpr(object, ident->getName());
        delete ident;
        return result;
    } else {
        return nullptr;
    }
}

Expr *
Parser::GetitemExpression(Expr &object)
{
    if (is(tok_lb)) {
        auto *expr = Expression();
        syntax(expr, "Expected key expression");
        syntax(is(tok_rb), "expected ']'");
        return new GetitemExpr(object, *expr);
    } else {
        return nullptr;
    }
}

IdentExpr *
Parser::Identifier()
{
    if (peek(tok_ident)) {
        auto &token = take();
        return new IdentExpr(token.getString());
    } else {
        return nullptr;
    }
}

Expr *
Parser::NumericLiteral()
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
Parser::StringLiteral()
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
Parser::Statement()
{
    return Statement(0);
}

Stmt *
Parser::Statement(int const outerIndent)
{
    Stmt *result = nullptr;
    if (EndOfFile()) return nullptr;
    int const indent = Indentation();
    syntax(indent == outerIndent, "Expected statement indentation");
    if (!result) result = SimpleStatement();
    if (!result) result = BlockStatement(indent);
    return result;
}

Stmt *
Parser::SimpleStatement()
{
    Stmt *result = nullptr;
    if (!result) result = BreakStatement();
    if (!result) result = ContinueStatement();
    if (!result) result = PassStatement();
    if (!result) result = ReturnStatement();
    if (!result) result = AssignStatement();
    if (!result) result = ExpressionStatement(); // Must be last.
    if ( result) syntax(is(tok_eol), "Expected end-of-statement");
    return result;
}

Stmt *
Parser::BreakStatement()
{
    if (is(kw_break)) {
        return new BreakStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser::ContinueStatement()
{
    if (is(kw_continue)) {
        return new ContinueStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser::PassStatement()
{
    if (is(kw_pass)) {
        return new PassStmt();
    } else {
        return nullptr;
    }
}

Stmt *
Parser::ExpressionStatement()
{
    if (auto *expr = Expression()) {
        return new ExprStmt(*expr);
    } else {
        return nullptr;
    }
}

Stmt *
Parser::ReturnStatement()
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
Parser::AssignStatement()
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
Parser::BlockStatement(int outerIndent)
{
    Stmt *result = nullptr;
    if (!result) result = IfStatement(outerIndent);
    if (!result) result = WhileStatement(outerIndent);
    if (!result) result = DefStatement(outerIndent);
    return result;
}

CompoundStmt *
Parser::CompoundStatement(int outerIndent)
{
    syntax(EndOfLine(), "Expected end-of-line");
    auto *result = new CompoundStmt();
    int const innerIndent = NextIndentation();
    syntax(innerIndent > outerIndent, "Expected block statement");

    do {
        int const indent = NextIndentation();

        if (indent <= outerIndent) {
            break;
        } else {
            syntax(indent == innerIndent, "Expected block indentation");
        }

        auto *statement = Statement(innerIndent);
        syntax(statement, "Expected statement");
        result->addStatement(*statement);
    } while (!EndOfFile());

    return result;
}

Stmt *
Parser::WhileStatement(int outerIndent)
{
    if (is(kw_while)) {
        auto *condition = Expression();
        syntax(condition, "Expected 'while' condition expression");
        syntax(is(tok_colon), "Expected ':'");
        auto *body = CompoundStatement(outerIndent);
        syntax(body, "Expected 'while' statement body");
        return new WhileStmt(*condition, *body);
    } else {
        return nullptr;
    }
}

Stmt *
Parser::IfStatement(int outerIndent)
{
    return IfStatement(outerIndent, false);
}

Stmt *
Parser::IfStatement(int outerIndent, bool isElif)
{
    if (is(isElif ? kw_elif : kw_if)) {
        Expr *condition = nullptr;
        CompoundStmt *thenBranch = nullptr;
        CompoundStmt *elseBranch = nullptr;

        condition = Expression();
        syntax(condition, "Expected 'if' condition expression");
        syntax(is(tok_colon), "Expected ':'");
        thenBranch = CompoundStatement(outerIndent);
        syntax(thenBranch, "Expected 'if' statement body");

        if (!EndOfFile()) {
            int const indent = Indentation();
            syntax(indent >= 0, "Expected indentation");

            if (indent == outerIndent && is(kw_else)) {
                syntax(is(tok_colon), "Expected ':'");
                elseBranch = CompoundStatement(outerIndent);
            } else if (indent == outerIndent && peek(kw_elif)) {
                auto *elifBranch = IfStatement(outerIndent, true);
                syntax(elifBranch, "Expected 'elif' branch");
                elseBranch = new CompoundStmt(*elifBranch);
            } else {
                back(); // Return the indentation.
            }
        }

        if (!elseBranch) {
            elseBranch = new CompoundStmt(*new PassStmt());
        }

        return new ConditionalStmt(*condition, *thenBranch, *elseBranch);
    } else {
        return nullptr;
    }
}

Stmt *
Parser::DefStatement(int outerIndent)
{
    if (is(kw_def)) {
        auto *name = Identifier();
        syntax(name, "Expected function name");
        syntax(is(tok_lp), "Expected '('");
        auto args = FunctionArguments();
        syntax(is(tok_rp), "Expected ')'");
        syntax(is(tok_colon), "Expected ':'");
        auto *body = CompoundStatement(outerIndent);
        syntax(body, "Expected compound statement");
        auto *result = new DefStmt(name->getName(), *body);

        for (auto &arg : args) {
            result->addArgument(arg);
        }

        delete name;
        return result;
    } else {
        return nullptr;
    }
}

void
Parser::next()
{
    assert(_index >= 0);
    assert(_index < _tokenCount);

    if (_index < _tokenCount - 1) {
        // Stop advancing once EOF is reached.
        _index++;
    }
}

void
Parser::back()
{
    assert(_index > 0);
    assert(_index < _tokenCount);
    _index--;
}

bool
Parser::is(TokenType const type)
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
Parser::peek() const
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
Parser::peek(TokenType type) const
{
    return peek().getTokenType() == type;
}

Token const &
Parser::take()
{
    auto &token = peek();
    next();
    return token;
}

int
Parser::precedence(Token const &token) const
{
    return precedence(token.getTokenType());
}

int
Parser::precedence(TokenType type) const
{
    assert(_precedence.count(type));
    return _precedence.at(type);
}

bool
Parser::isLeftAssociative(Token const &token) const
{
    return isLeftAssociative(token.getTokenType());
}

bool
Parser::isLeftAssociative(TokenType type) const
{
    return ! (type & tok_rassoc);
}

void
Parser::syntax(bool condition, std::string const &message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

CompoundStmt &
Parser::Module()
{
    auto *result = new CompoundStmt();

    while (auto *stmt = Statement()) {
        result->addStatement(*stmt);
    }

    return *result;
}
