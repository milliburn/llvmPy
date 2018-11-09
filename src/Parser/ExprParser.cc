#include <llvmPy/Parser/ExprParser.h>
using namespace llvmPy;

std::unique_ptr<Expr>
ExprParser::fromIter(ExprParser::TTokenIter iter)
{
    ExprParser parser(iter);
    return parser.parse();
}

ExprParser::ExprParser(ExprParser::TTokenIter iter)
: iter(iter)
{
}

std::unique_ptr<Expr>
ExprParser::parse()
{
    std::next(iter);
    return std::unique_ptr<Expr>();
}
