#pragma once
#include <exception>
#include <string>

#ifdef __cplusplus
namespace llvmPy {
namespace AST {

class SyntaxError : std::runtime_error {
public:
    explicit SyntaxError(std::string const &);
};

} // namespace AST
} // namespace llvmPy
#endif // __cplusplus
