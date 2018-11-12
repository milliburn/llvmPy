#include <llvmPy/Parser2.h>
#include <llvm/Support/Casting.h>
using namespace llvmPy;
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

        if (isEnd() || is(tok_colon)) {
            isTerminal = true;
        } else if (term && is(term)) {
            next();
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
    Expr *rhs = nullptr;
    TokenExpr *binaryOp = nullptr;
    TokenExpr *unaryOp = nullptr;
    int curPrec = 0;

    if (isEnd() || is(tok_rp)) {
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
    } else if ((binaryOp = findOperator())) {
        curPrec = getPrecedence(binaryOp);

        if (curPrec <= lastPrec) {
            back();
            return lhs;
        }

        unaryOp = findOperator();
    }

    if (is(tok_lp)) {
        rhs = readSubExpr();
        rhs = readExpr(curPrec, rhs);
    } else if (
            (rhs = findNumericLiteral()) ||
            (rhs = findStringLiteral()) ||
            (rhs = findIdentifier()) ||
            (rhs = findLambdaExpression())) {
        rhs = readExpr(curPrec, rhs);
    }

    if (unaryOp && rhs) {
        rhs = new UnaryExpr(
                unaryOp->getTokenType(),
                std::unique_ptr<Expr>(rhs));
    }

    if (lhs && binaryOp && rhs) {
        // Apply binary operator to LHS and RHS.
        Expr *binop = new BinaryExpr(lhs, binaryOp->getTokenType(), rhs);
        return readExpr(lastPrec, binop);
    } else if (lhs && !binaryOp && rhs) {
        // Function call (apply arguments in RHS to callee in LHS).
        return buildCall(lhs, rhs);
    } else if (lhs && !binaryOp && !rhs) {
        // Independent expression (generally literal or identifier).
        return lhs;
    } else if (!lhs && binaryOp && rhs) {
        return new UnaryExpr(
                binaryOp->getTokenType(),
                std::unique_ptr<Expr>(rhs));
    } else if (!lhs && !binaryOp) {
        return rhs;
    } else {
        throw "Not implemented";
    }
}

Stmt *
Parser2::readStatement()
{
    if (auto *stmt = findDefStatement(0)) {
        return stmt;
    } else if (auto *stmt = findReturnStatement()) {
        return stmt;
    } else {
        return nullptr;
    }
}

CallExpr *
Parser2::buildCall(Expr *lhs, Expr *rhs)
{
    auto *call = new CallExpr(std::unique_ptr<Expr>(lhs));

    if (auto *tuple = dyn_cast<TupleExpr>(rhs)) {
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
    return token().type == tokenType;
}

bool
Parser2::is_a(TokenType tokenType)
{
    return token().type & tokenType;
}

bool
Parser2::isEnd()
{
    return iter == iter_end || (*iter).type == tok_eof;
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

Token const &
Parser2::token() const
{
    return *iter;
}

Expr *
Parser2::findNumericLiteral()
{
    if (is(tok_number)) {
        std::string const *text = token().str;
        next();

        if (text->find('.') != std::string::npos) {
            double value = atof(text->c_str());
            return new DecLitExpr(value);
        } else {
            int64_t value = atol(text->c_str());
            return new IntLitExpr(value);
        }
    } else {
        return nullptr;
    }
}

StrLitExpr *
Parser2::findStringLiteral()
{
    if (is(tok_string)) {
        // The substring removes string delimiters (" or ').
        std::string const *str = token().str;
        auto value = std::make_unique<std::string const>(
                str->substr(1, str->length() - 2));
        next();
        return new StrLitExpr(std::move(value));
    } else {
        return nullptr;
    }
}

IdentExpr *
Parser2::findIdentifier()
{
    if (is(tok_ident)) {
        auto *expr = new IdentExpr(token().str);
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
        TokenType tokenType = token().type;
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

        if (auto *tuple = dyn_cast<TupleExpr>(argsSubExpr)) {
            for (auto const &member : tuple->getMembers()) {
                auto const &ident = llvm::cast<IdentExpr>(*member);
                argNames.push_back(ident.getName());
            }
        } else if (auto *ident = dyn_cast<IdentExpr>(argsSubExpr)) {
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
        auto *functionName = llvm::cast<IdentExpr>(readExpr(0, nullptr));
        std::vector<std::string const> argNames;

        assert(is(tok_lp));
        next();

        auto *argsSubExpr = readSubExpr();

        assert(is(tok_rp));
        next();

        if (auto *tuple = dyn_cast<TupleExpr>(argsSubExpr)) {
            for (auto const &member : tuple->getMembers()) {
                auto const &ident = llvm::cast<IdentExpr>(*member);
                argNames.push_back(ident.getName());
            }
        } else if (auto *ident = dyn_cast<IdentExpr>(argsSubExpr)) {
            argNames.push_back(ident->getName());
        } else {
            assert(false && "Invalid argument subexpression");
        }

        delete(argsSubExpr);

        assert(is(tok_colon));
        next();

        assert(is(tok_eol));
        next();

        CompoundStmt *body = readCompoundStatement(outerIndent);
        assert(body);

        return new DefStmt(
                functionName->getName(),
                std::move(argNames),
                std::unique_ptr<CompoundStmt>(body));
    } else {
        return nullptr;
    }
}

CompoundStmt *
Parser2::readCompoundStatement(int outerIndent)
{
    auto *compound = new CompoundStmt();

    // A compound statement _must_ contain at least one inner statement,
    // or otherwise `pass`.

    assert(is(tok_indent));
    auto const innerIndent = static_cast<int>(token().depth);
    assert(innerIndent > outerIndent);
    back(); // To simplify the loop.

    for (;;) {
        if (isEnd()) {
            break;
        }

        assert(is(tok_indent));
        auto const indent = static_cast<int>(token().depth);
        next();

        if (is(tok_eol)) {
            // Ignore indents if the line is otherwise empty.
            next();
        } else if (indent <= outerIndent) {
            // End of statement.
            back();
            break;
        } else if (indent == innerIndent) {
            auto *stmt = readStatement();
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
        auto *expr = readExpr();
        assert(expr);
        return new ReturnStmt(*expr);
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
