#pragma once
#include <llvmPy/RTAtom.h>
#include <llvmPy/RTObj.h>
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class Value;
} // namespace llvm

namespace llvmPy {

class RTScope {
public:
    RTScope() : RTScope(nullptr) {}

    explicit inline
    RTScope(RTScope *parent) : parent(parent) {}

    RTScope * const parent;
    std::unordered_map<std::string, llvm::Value *> slots;
};

class RTModule {
    RTModule(llvm::Module *ir) : ir(ir) {}
    llvm::Module * const ir;
};

class RT {
public:
    std::unordered_map<std::string, RTModuleObj *> modules;
};

} // namespace llvmPy
#endif // __cplusplus

