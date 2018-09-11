#pragma once
#include <llvmPy/AST/Token.h>
#include <array>
#include <string>
#include <utility>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

constexpr int OPERATORS_COUNT = 6;
constexpr int KEYWORDS_COUNT = 8;
constexpr int SYNTAXES_COUNT = 6;

extern
std::array<
        std::pair<
                std::string const,
                OperType> const,
        OPERATORS_COUNT> const
OPERATORS;

constexpr char const * OP_CHAR1 = "+-*/!=";
constexpr char const * OP_CHAR2 = "     =";

extern
std::array<
        std::pair<
                std::string const,
                KeywType> const,
        KEYWORDS_COUNT> const
KEYWORDS;

extern
std::array<
        std::pair<
                std::string const,
                SyntaxType> const,
        SYNTAXES_COUNT> const
SYNTAXES;

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
