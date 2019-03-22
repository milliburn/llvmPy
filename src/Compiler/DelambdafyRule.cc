#include <assert.h>
#include <llvmPy/Compiler/Parser/DelambdafyRule.h>
using namespace llvmPy;

void
DelambdafyRule::updateLambdaExpr(LambdaExpr &lambda)
{
    //
    // Generate a def-statement equivalent to the lambda expression.
    //

    auto &expr = lambda.getExpr();
    auto &stmt = *new CompoundStmt(*new ReturnStmt(expr));

    auto &function = *new DefStmt("__lambda__", stmt);
    auto &functionReference = *new IdentExpr(function.getName());

    for (auto &arg : lambda.args()) {
        function.addArgument(arg);
    }

    //
    // Place the generated def-statement before the enclosing statement.
    //

    auto &block = getParentOfType<CompoundStmt>(lambda);
    auto &enclosing = getParentOfType<Stmt>(lambda);
    block.insertBefore(&enclosing, function);

    //
    // Replace the original lambda reference with that of the def-statement.
    //

    auto &parent = lambda.getParent();
    auto *oldval = parent.replace(lambda, functionReference);
    delete oldval;
}
