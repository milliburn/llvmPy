#pragma once
#include <exception>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class ParserError : std::runtime_error {
public:
    explicit ParserError(std::string const &);
};

class SyntaxError : std::runtime_error {
public:
    explicit SyntaxError(std::string const &);
};

} // namespace llvmPy
#endif // __cplusplus
