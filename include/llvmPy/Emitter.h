#pragma once
#include <llvmPy/AST.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#ifdef __cplusplus
namespace llvmPy {

class RT;
class RTScope;

class Emitter {
public:
    Emitter(RT &);
    llvm::Value * emit(AST const &, RTScope &);

private:
    RT &rt;
    llvm::LLVMContext ctx;
    llvm::IRBuilder<> ir;
};

} // namespace llvmPy
#endif // __cplusplus
