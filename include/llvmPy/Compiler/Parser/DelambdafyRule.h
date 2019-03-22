#pragma once
#include <llvmPy/AST.h>
#include <llvmPy/Compiler/Parser/AstPass.h>

#ifdef __cplusplus
namespace llvmPy {

class DelambdafyRule : public AstPass {
public:
    DelambdafyRule();

protected:
    void updateLambdaExpr(LambdaExpr &lambda) override;
};


} // namespace llvmPy
#endif // __cplusplus
