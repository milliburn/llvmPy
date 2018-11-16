#include <llvmPy/Parser2.h>
using namespace llvmPy;

// The "amount" of precedence to add for unary operators.
static constexpr int UnaryPrecedenceOffset = 1;

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

std::unique_ptr<Expr>
Parser2::fromIter(
        Parser2::TTokenIter &iter,
        Parser2::TTokenIter end)
{
    Parser2 parser(iter, std::move(end));
    return parser.parse();
}

Parser2::Parser2(
        Parser2::TTokenIter &iter,
        Parser2::TTokenIter end)
: iter(iter), iter_end(end), precedences(initPrecedence())
{
}

std::unique_ptr<Expr>
Parser2::parse()
{
    Expr *result = readExpr(0, nullptr);
    if (!result) result = new TupleExpr();
    return std::unique_ptr<Expr>(result);
}

std::unique_ptr<Stmt>
Parser2::read()
{
    auto compound = std::make_unique<CompoundStmt>();

    for (;;) {
        if (auto *stmt = readStatement(0)) {
            compound->addStatement(std::unique_ptr<Stmt>(stmt));
        } else {
            break;
        }
    }

    return std::move(compound);
}

Expr *
Parser2::readSubExpr()
{
    TokenType term = tok_unknown;

    if (is(tok_lp)) {
        next();
        term = tok_rp;
    }

    TupleExpr *tuple = nullptr;

    for (;;) {
        auto *expr = readExpr(0, nullptr);
        bool isTerminal = false;

        if (term && is(term)) {
            next();
            isTerminal = true;
        } else if (!expr || isEnd() || is(tok_colon) || is(tok_eol)) {
            isTerminal = true;
        }

        if (isTerminal) {
            if (tuple) {
                if (expr) {
                    tuple->addMember(std::unique_ptr<Expr>(expr));
                }

                return tuple;
            } else if (!expr) {
                return new TupleExpr();
            } else {
                return expr;
            }
        } else if (is(tok_comma)) {
            // This subexpression is a tuple.
            next();

            if (!tuple) {
                tuple = new TupleExpr();
            }

            tuple->addMember(std::unique_ptr<Expr>(expr));
        }
    }
}

Expr *
Parser2::readExpr(int lastPrec, Expr *lhs)
{
    if (isEnd() || is(tok_rp) || is(tok_eol)) {
        // End of (sub)expression.
        // Terminator (if any) is intentionally left un-consumed.
        return lhs;
    } else if (is(tok_colon)) {
        // The colon is used in lambda-expressions and
        // as a statement terminator.
        return lhs;
    } else if (is(tok_comma)) {
        // Comma is a delimiter (no binding) between expressions.
        // It is intentionally left un-consumed.
        return lhs;
    }

    if (lhs) {
        // The expression is being read in a binary context.

        if (auto *op = findOperator()) {

            int precedence = getPrecedence(op);

            if (precedence <= lastPrec) {
                back();
                return lhs;
            } else {

                Expr *rhs = nullptr;

                if ((rhs = readAtomicExpr())) {
                    // Ignore
                } else if (is(tok_lp)) {
                    rhs = readSubExpr();
                } else if (findOperator()) {
                    back();
                    rhs = readExpr(precedence, nullptr);
                } else {
                    assert(false && "Unhandled state");
                    return nullptr;
                }

                assert(rhs);
                rhs = readExpr(precedence, rhs);
                assert(rhs);

                auto *binary = new BinaryExpr(lhs, op->getTokenType(), rhs);
                return readExpr(lastPrec, binary);

            }

        } else if (is(tok_lp)) {

            auto *args = readSubExpr();
            auto *call = buildCall(lhs, args);
            return readExpr(lastPrec, call);

        } else {

            assert(false && "Unhandled state");

        }

    } else {
        // It's the start of the (sub)expression.

        if (auto *op = findUnaryOperator()) {

            // TODO: Is this the correct application of unary precedence?
            int precedence = getPrecedence(op) + UnaryPrecedenceOffset;
            auto *operand = readExpr(precedence, nullptr);
            assert(operand);
            auto *unary = new UnaryExpr(
                    op->getTokenType(),
                    std::unique_ptr<Expr>(operand));
            return readExpr(lastPrec, unary);

        } else if (auto *atomic = readAtomicExpr()) {

            return readExpr(lastPrec, atomic);

        } else if (is(tok_lp)) {

            // TODO: Is this the correct application of call precedence?
            auto *subExpr = readSubExpr();
            assert(subExpr);
            return readExpr(lastPrec, subExpr);

        } else {

            return nullptr;

        }
    }
}

Expr *
Parser2::readAtomicExpr()
{
    Expr *rhs = nullptr;
    (
            (rhs = findNumericLiteral()) ||
            (rhs = findStringLiteral()) ||
            (rhs = findIdentifier()) ||
            (rhs = findLambdaExpression()));
    return rhs;
}

/**
 * @brief Read a simple or block statement.
 *
 * A statement _must_ start with an indentation followed by either a simple or
 * block statement. A statement concludes with either EOL or EOF, although
 * statements may themselves consume additional EOLs (in case of blocks). The
 * applicable read___() method _must_ consume a terminating EOL, but not a
 * terminating EOF.
 *
 * @param indent Indentation level of the current compound statement.
 *        Pass 0 when reading modules.
 */
Stmt *
Parser2::readStatement(int indent)
{
    Stmt *stmt = nullptr;

    // Pass by and ignore any empty lines.
    for (;;) {
        if (is(tok_indent)) {
            next();
            if (is(tok_eol)) {
                next();
                continue;
            } else if (is(tok_eof)) {
                return nullptr;
            } else {
                back();
                expectIndent(indent);
                break;
            }
        } else if (is(tok_eof)) {
            return nullptr;
        } else {
            break;
        }
    }

    if (auto *simple = readSimpleStatement(indent)) {
        stmt = simple;
    } else if (auto *block = readBlockStatement(indent)) {
        stmt = block;
    } else {
        assert(false && "Unexpected statement");
        return nullptr;
    }

    while (is(tok_eol)) {
        next();
    }

    return stmt;
}

/**
 * @brief Read an one-line statement such as "return 1" or "print('foo')".
 *
 * The lower-level parsers are expected to not consume EOLs unless the statement
 * is broken into multiple lines by means of a \ (not implemented).
 */
Stmt *
Parser2::readSimpleStatement(int indent)
{
    CompoundStmt *compound = nullptr;
    Stmt *stmt = nullptr;

    for (;;) {
        if (stmt) {
            assert(compound);
            compound->addStatement(std::unique_ptr<Stmt>(stmt));
            stmt = nullptr;
        }

        if (is(tok_eol)) {
            next();
            break;
        } else if (is(tok_eof)) {
            break;
        }

        if ((stmt = findReturnStatement()) ||
            (stmt = findAssignStatement()) ||
            (stmt = findPassStatement()) ||
            (stmt = findBreakStmt()) ||
            (stmt = findContinueStmt())) {
        } else if (auto *expr = readExpr()) {
            stmt = new ExprStmt(expr);
        } else {
            return nullptr;
        }

        if (is(tok_semicolon)) {
            // Successive statements will acquire the original's indent.
            next();

            if (!compound) {
                compound = new CompoundStmt();
            }
        } else {
            break;
        }
    }

    if (compound) {
        return compound;
    } else {
        return stmt;
    }
}

/**
 * @brief Read a block statement consisting of a header and compound statement.
 *
 * The header of a block statement must be at the `indent` level, whereas
 * the compound statement must have greater indentation.
 */
Stmt *
Parser2::readBlockStatement(int indent)
{
    if (auto *def = findDefStatement(indent)) {
        return def;
    } else if (auto *cond = findConditionalStatement(indent, false)) {
        return cond;
    } else if (auto *while_ = findWhileStmt(indent)) {
        return while_;
    } else {
        return nullptr;
    }
}

CallExpr *
Parser2::buildCall(Expr *lhs, Expr *rhs)
{
    auto *call = new CallExpr(std::unique_ptr<Expr>(lhs));

    if (auto *tuple = rhs->cast<TupleExpr>()) {
        for (auto &member : tuple->getMembers()) {
            call->addArgument(std::move(member));
        }

        delete(tuple);
    } else if (rhs) {
        call->addArgument(std::unique_ptr<Expr>(rhs));
    }

    return call;
}

bool
Parser2::is(TokenType tokenType)
{
    return token().getTokenType() == tokenType;
}

bool
Parser2::is_a(TokenType tokenType)
{
    return token().getTokenType() & tokenType;
}

bool
Parser2::isEnd()
{
    return iter == iter_end || iter->getTokenType() == tok_eof;
}

void
Parser2::next()
{
    if (!isEnd()) {
        iter++;
    }
}

void
Parser2::back()
{
    iter--;
}

Token &
Parser2::token() const
{
    return *iter;
}

Expr *
Parser2::findNumericLiteral()
{
    if (is(tok_number)) {
        auto const &text = token().getString();
        next();

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

StringExpr *
Parser2::findStringLiteral()
{
    if (is(tok_string)) {
        // The substring removes string delimiters (" or ').
        auto const &text = token().getString();
        auto value = std::make_unique<std::string const>(
                text.substr(1, text.length() - 2));
        next();
        return new StringExpr(std::move(value));
    } else {
        return nullptr;
    }
}

IdentExpr *
Parser2::findIdentifier()
{
    if (is(tok_ident)) {
        auto name = token().releaseString();
        auto *expr = new IdentExpr(std::move(name));
        next();
        return expr;
    } else {
        return nullptr;
    }
}

TokenExpr *
Parser2::findOperator()
{
    if (is_a(tok_oper)) {
        TokenType tokenType = token().getTokenType();
        next();
        return new TokenExpr(tokenType);
    } else {
        return nullptr;
    }
}

LambdaExpr *
Parser2::findLambdaExpression()
{
    if (is(kw_lambda)) {
        next();

        std::vector<std::string const> argNames;

        Expr *argsSubExpr = readSubExpr();

        if (auto *tuple = argsSubExpr->cast<TupleExpr>()) {
            for (auto const &member : tuple->getMembers()) {
                auto const &ident = member->as<IdentExpr>();
                argNames.push_back(ident.getName());
            }
        } else if (auto *ident = argsSubExpr->cast<IdentExpr>()) {
            argNames.push_back(ident->getName());
        } else {
            assert(false && "Invalid argument subexpression");
        }

        delete(argsSubExpr);

        assert(is(tok_colon));
        next();

        Expr *lambdaBody = readSubExpr();

        return new LambdaExpr(argNames, lambdaBody);
    } else {
        return nullptr;
    }
}

DefStmt *
Parser2::findDefStatement(int outerIndent)
{
    if (is(kw_def)) {
        next();

        assert(is(tok_ident));
        auto fnName = token().releaseString();
        next();

        assert(is(tok_lp));

        auto fnSignatureExpr = std::unique_ptr<Expr>(readSubExpr());
        std::unique_ptr<TupleExpr> fnSignatureTuple;

        if (fnSignatureExpr->isa<IdentExpr>()) {
            fnSignatureTuple = std::make_unique<TupleExpr>();
            fnSignatureTuple->addMember(std::move(fnSignatureExpr));
        } else {
            assert(fnSignatureExpr->isa<TupleExpr>());
            fnSignatureTuple.reset(
                    fnSignatureExpr.release()->cast<TupleExpr>());
        }

        assert(is(tok_colon));
        next();

        std::vector<std::string const> argNames;

        for (auto &arg : fnSignatureTuple->getMembers()) {
            auto const &ident = arg->as<IdentExpr>();
            argNames.push_back(ident.getName());
        }

        assert(is(tok_eol));
        next();

        CompoundStmt *body = readCompoundStatement(outerIndent);
        assert(body);

        return new DefStmt(
                std::move(fnName),
                std::move(argNames),
                std::unique_ptr<CompoundStmt>(body));
    } else {
        return nullptr;
    }
}

/**
 * @brief Read a compound statement.
 *
 * A compound statement consists of one or more statements at an equal level
 * of indentation. The indentation of the inner statements _must_ be greater
 * than `outerIndent`, and must remain stable throughout.
 *
 * A compound statement concludes when EOF, or otherwise when the indentation
 * level of a following line is less than or equal to the `outerIndent`.
 *
 * @param outerIndent The indentation level of the enclosing block header
 *        (e.g. the if-statement).
 */
CompoundStmt *
Parser2::readCompoundStatement(int outerIndent)
{
    auto *compound = new CompoundStmt();

    // A compound statement _must_ contain at least one inner statement,
    // or otherwise `pass`.

    assert(is(tok_indent));
    auto const innerIndent = static_cast<int>(token().getDepth());
    assert(innerIndent > outerIndent);

    for (;;) {
        if (is(tok_eof)) {
            break;
        }

        assert(is(tok_indent));
        auto const indent = static_cast<int>(token().getDepth());
        next();

        if (is(tok_eol)) {
            // Ignore indents if the line is otherwise empty.
            next();
        } else if (indent <= outerIndent) {
            // End of statement.
            back();
            break;
        } else if (indent == innerIndent) {
            auto *stmt = readStatement(innerIndent);

            if (!stmt) {
                break;
            }

            assert(stmt);
            compound->addStatement(std::unique_ptr<Stmt>(stmt));
        } else {
            // Covers cases where:
            //   (a) outerIndent < indent < innerIndent
            //   (b) indent > innerIndent
            assert(false && "Unexpected indent");
            return nullptr;
        }
    }

    return compound;
}

ReturnStmt *
Parser2::findReturnStatement()
{
    if (is(kw_return)) {
        next();
        auto *expr = readExpr();
        assert(expr);
        return new ReturnStmt(*expr);
    } else {
        return nullptr;
    }
}

AssignStmt *
Parser2::findAssignStatement()
{
    if (is(tok_ident)) {
        auto &nameToken = token();
        next();

        if (is_a(tok_assign)) {
            next();
            auto *expr = readExpr();
            assert(expr);
            auto name = nameToken.releaseString();
            return new AssignStmt(
                    std::move(name),
                    std::unique_ptr<Expr const>(expr));
        } else {
            back();
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

int
Parser2::getPrecedence(TokenType tokenType) const
{
    if (precedences.count(tokenType)) {
        return precedences.at(tokenType);
    } else {
        return 0;
    }
}

int
Parser2::getPrecedence(TokenExpr *tokenExpr) const
{
    return getPrecedence(tokenExpr->getTokenType());
}

void
Parser2::expectIndent(int indent)
{
    assert(is(tok_indent));
    int const actual = static_cast<int>(token().getDepth());
    assert(indent == actual && "Unexpected indent");
    next();
}

ConditionalStmt *
Parser2::findConditionalStatement(int outerIndent, bool elif)
{
    if (is(elif ? kw_elif : kw_if)) {
        next();

        auto *condition = readExpr();
        assert(condition);

        assert(is(tok_colon));
        next();

        assert(is(tok_eol));
        next();

        Stmt *thenBranch = nullptr;
        Stmt *elseBranch = nullptr;

        thenBranch = readCompoundStatement(outerIndent);
        assert(thenBranch);

        if (isAtIndent(kw_else, outerIndent)) {
            next();

            assert(is(tok_colon));
            next();

            assert(is(tok_eol));
            next();

            elseBranch = readCompoundStatement(outerIndent);
        } else if (isAtIndent(kw_elif, outerIndent)) {
            elseBranch = findConditionalStatement(outerIndent, true);
        } else {
            elseBranch = new PassStmt();
        }

        assert(elseBranch);

        auto *condStmt = new ConditionalStmt(
                std::unique_ptr<Expr>(condition),
                std::unique_ptr<Stmt>(thenBranch),
                std::unique_ptr<Stmt>(elseBranch));

        return condStmt;
    } else {
        return nullptr;
    }
}

PassStmt *
Parser2::findPassStatement()
{
    if (is(kw_pass)) {
        next();
        return new PassStmt();
    } else {
        return nullptr;
    }
}

bool
Parser2::isAtIndent(TokenType const tokenType, int const indent)
{
    if (is(tok_indent)) {
        int const nextIndent = static_cast<int>(token().getDepth());
        next();
        if (nextIndent == indent && is(tokenType)) {
            return true;
        } else {
            back();
            return false;
        }
    } else {
        return false;
    }
}

WhileStmt *
Parser2::findWhileStmt(int outerIndent)
{
    if (is(kw_while)) {
        next();
        auto *condition = readExpr();
        assert(condition);

        assert(is(tok_colon));
        next();

        assert(is(tok_eol));
        next();

        Stmt *body = readCompoundStatement(outerIndent);
        assert(body);

        return new WhileStmt(
                std::unique_ptr<Expr>(condition),
                std::unique_ptr<Stmt>(body));
    } else {
        return nullptr;
    }
}

BreakStmt *
Parser2::findBreakStmt()
{
    if (is(kw_break)) {
        next();
        return new BreakStmt();
    } else {
        return nullptr;
    }
}

ContinueStmt *
Parser2::findContinueStmt()
{
    if (is(kw_continue)) {
        next();
        return new ContinueStmt();
    } else {
        return nullptr;
    }
}

TokenExpr *
Parser2::findUnaryOperator()
{
    if (auto *op = findOperator()) {
        switch (op->getTokenType()) {
        case tok_add:
        case tok_sub:
            return op;

        default:
            back();
            return nullptr;
        }
    } else {
        return nullptr;
    }
}
