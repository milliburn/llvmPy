#pragma once

#include <llvmPy.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvmPy {

std::vector<Stmt *> parseFile(std::string const &path);
std::string readFile(std::string const &path);
std::string emitFile(std::string const &path);

} // namespace llvmPy
#endif // __cplusplus
