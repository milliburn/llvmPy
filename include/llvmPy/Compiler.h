#pragma once

#ifdef __cplusplus
namespace llvm {
class Module;
} // namespace llvm

namespace llvmPy {

class RT;

class Compiler {
public:
    explicit Compiler(RT &rt) noexcept : rt(rt) {}
    void add(llvm::Module *);
    void addMain(llvm::Module *);

private:
    RT &rt;
};

} // namespace llvmPy
#endif // __cplusplus
