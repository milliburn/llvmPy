#pragma once
#include <exception>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class SyntaxError : std::runtime_error {
public:
    explicit SyntaxError(std::string const &);
};

} // namespace llvmPy
#endif // __cplusplus
