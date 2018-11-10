#include <llvmPy/Parser/ExprParser.h>
#include <llvm/Support/Casting.h>
using namespace llvmPy;

std::unique_ptr<Expr>
ExprParser::fromIter(
        ExprParser::TTokenIter iter,
        ExprParser::TTokenIter end)
{
    ExprParser parser(std::move(iter), std::move(end));
    return parser.parse();
}

ExprParser::ExprParser(
        ExprParser::TTokenIter &&iter,
        ExprParser::TTokenIter &&end)
: iter(iter), iter_end(end)
{
}

std::unique_ptr<Expr>
ExprParser::parse()
{
    while (!end()) {
        consume();
    }

    while (!operators.empty()) {
        output.push(operators.top());
        operators.pop();
    }

    while (!output.empty()) {
        evaluate();
    }

    std::unique_ptr<Expr> rv(result.top());
    return rv;
}

void
ExprParser::consume()
{
    if (auto *lit = findIntegerLiteral()) {
        output.push(lit);
    } else if (auto *ident = findIdentifier()) {
        output.push(ident);
    } else if (auto *token = findOperator()) {
        operators.push(token);
    }
}

void
ExprParser::evaluate()
{
    // while (!output.empty()) {
    //     auto expr = output.front();
    //     output.pop_front();
    //
    //     if (llvm::isa<LitExpr>(expr) || llvm::isa<IdentExpr>(expr)) {
    //         result.push(expr);
    //         break;
    //     } else if (auto *op = llvm::dyn_cast<TokenExpr>(expr)) {
    //         auto a = result.top(); result.pop();
    //         auto b = result.top(); result.pop();
    //         result.push(new BinaryExpr(a, op->getTokenType(), b));
    //         break;
    //     }
    // }
}

bool
ExprParser::is(TokenType tokenType)
{
    return token().type == tokenType;
}

bool
ExprParser::is_a(TokenType tokenType)
{
    return token().type & tokenType;
}

bool
ExprParser::end()
{
    return iter == iter_end || (*iter).type == tok_eof;
}

void
ExprParser::next()
{
    iter++;
}

void
ExprParser::back()
{
    iter--;
}

Token const &
ExprParser::token() const
{
    return *iter;
}

std::stack<Expr *> const &
ExprParser::getOutput() const
{
    return output;
}

std::stack<Expr *> const &
ExprParser::getOperators() const
{
    return operators;
}

IntLitExpr *
ExprParser::findIntegerLiteral()
{
    int sign = 0;

    if (is(tok_add)) {
        sign = 1;
        next();
    } else if (is(tok_sub)) {
        sign = -1;
        next();
    }

    if (is(tok_number)) {
        std::string const *text = token().str;
        int64_t value = atol(text->c_str());
        next();
        return new IntLitExpr(sign < 0 ? -value : value);
    }

    back();
    return nullptr;
}

IdentExpr *
ExprParser::findIdentifier()
{
    if (is(tok_ident)) {
        return new IdentExpr(token().str);
    }

    return nullptr;
}

TokenExpr *
ExprParser::findOperator()
{
    if (is_a(tok_oper)) {
        TokenType tokenType = token().type;
        return new TokenExpr(tokenType);
    }

    return nullptr;
}
