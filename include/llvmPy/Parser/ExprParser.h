#pragma once

#include <llvmPy/Token.h>
#include <llvmPy/AST.h>
#include <vector>
#include <memory>
#include <istream>

#ifdef __cplusplus
namespace llvmPy {

/**
 * This class is specialized towards parsing the AST of a single expression
 * (which may be a complex tree) out of a stream of tokens.
 */
class ExprParser {
public:
    typedef std::vector<Token>::iterator const & TTokenIter;

    static std::unique_ptr<Expr> fromIter(TTokenIter iter);

    explicit ExprParser(TTokenIter tokens);
    std::unique_ptr<Expr> parse();

private:
    TTokenIter iter;
};

} // namespace llvmPy
#endif // __cplusplus
