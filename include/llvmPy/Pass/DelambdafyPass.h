#pragma once

#include "llvmPy/Pass.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class Stmt;

class DelambdafyPass : public Pass<Stmt const, Stmt const> {
public:
    
};

} // namespace llvmPy
#endif // __cplusplus
