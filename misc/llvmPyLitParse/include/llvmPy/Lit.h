#pragma once

#include <llvmPy/LitParser.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

int lit(std::vector<LitTestResult *> &results,
        std::string const &path);

} // namespace
#endif // __cplusplus
#pragma once
