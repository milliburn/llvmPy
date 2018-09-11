#pragma once
#include <llvmPy/AST/Token.h>
#include <array>
#include <string>
#include <utility>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

constexpr int OPERATORS_COUNT = 4;
constexpr int KEYWORDS_COUNT = 8;
constexpr int SYNTAXES_COUNT = 5;

extern
std::array<
        std::pair<
                std::string const,
                OperType> const,
        OPERATORS_COUNT> const
OPERATORS;

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
