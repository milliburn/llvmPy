#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#ifdef __cplusplus
namespace llvm {
class BasicBlock;
class Value;
} // namespace llvm

namespace llvmPy {

class RTValue {
public:
    llvm::Value *ir = nullptr;
};

class RT;

class RTScope {
public:
    RTScope(RT &rt) : rt(rt) {}

    RT &rt;
    RTScope *parent = nullptr;
    std::unordered_map<std::string, RTValue *> slots;
    // std::vector<llvm::BasicBlock *> text;
};

class RTFunc : public RTScope {
public:
    std::unordered_map<std::string, RTValue *> args;
};

class RT {
public:
    std::unordered_map<std::string, RTScope *> modules;
    void setMainModule(RTScope *);
    RTScope * createScope();
};

} // namespace llvmPy
#endif // __cplusplus

