#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::string;
using std::to_string;
using std::vector;

Emitter::Emitter(Compiler &c) noexcept
: rt(c.getRT()),
  dl(c.getDataLayout()),
  ctx(c.getContext()),
  ir(ctx),
  types(ctx, dl)
{
}

RTModule *
Emitter::createModule(std::string const &name)
{
    auto *module = new llvm::Module(name, ctx);

    vector<llvm::Type *> argTypes;

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            llvm::Type::getVoidTy(ctx),
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    "__body__",
                    module);

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "", func);

    return new RTModule(name, module, types, func);
}

llvm::Value *
Emitter::emit(RTModule &mod, AST const &ast)
{
    switch (ast.getType()) {
    case ASTType::ExprIntLit: {
        auto &expr = cast<IntLitExpr>(ast);
        auto *value = llvm::ConstantInt::get(
                types.PyIntValue,
                static_cast<uint64_t>(expr.value));
        return ir.CreateCall(mod.llvmPy_int(), { value });
    }

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = mod.getScope().slots[ident];
        auto *rhs = emit(mod, stmt.rhs);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(mod, expr.expr);
    }

    default:
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTModule &mod, std::vector<Stmt *> const &stmts)
{
    llvm::Value *lastValue = nullptr;

    // Store original insert point.
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();

    // Begin inserting into module body.
    llvm::BasicBlock *bb = &mod.getFunction().back();

    for (auto *stmt : stmts) {
        if (stmt->getType() == ASTType::StmtAssign) {
            ir.SetInsertPoint(bb);
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            auto *alloca = ir.CreateAlloca(types.Ptr);
            mod.getScope().slots[ident] = alloca;

            ir.CreateStore(
                    llvm::ConstantPointerNull::get(alloca->getType()),
                    alloca);
        }
    }

    for (auto *stmt : stmts) {
        ir.SetInsertPoint(bb);
        lastValue = emit(mod, *stmt);
    }

    // Restore original insert point.
    ir.SetInsertPoint(insertPoint);

    return lastValue;
}
