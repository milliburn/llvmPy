#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <stdlib.h>
using namespace llvmPy;
using llvm::cast;
using llvm::isa;
using std::make_pair;
using std::string;
using std::to_string;
using std::vector;
using std::cerr;
using std::endl;

static struct {
    string FuncObj = "";
    string PosArgCount = "";
    string FuncPtr = "";
    string RetVal = "";
    string Arg = "";
    string OuterFrame = "outer";
    string InnerFrame = "frame";
    string CallFrame = "callframe";
} tags;

Emitter::Emitter(Compiler &c) noexcept
: rt(c.getRT()),
  dl(c.getDataLayout()),
  ctx(c.getContext()),
  ir(ctx),
  types(ctx, dl)
{
}

RTModule *
Emitter::createModule(
        std::string const &name,
        std::vector<Stmt *> const &stmts)
{
    auto *module = new llvm::Module(name, ctx);
    RTModule *rtModule = new RTModule(name, module, types);
    RTFunc *body = createFunction("__body__", rtModule->getScope(), stmts);
    rtModule->setBody(body);
    return rtModule;
}

llvm::Value *
Emitter::emit(RTScope &scope, AST const &ast)
{
    RTModule &mod = scope.getModule();

    switch (ast.getType()) {
    case ASTType::ExprIntLit: return emit(scope, cast<IntLitExpr>(ast));
    case ASTType::ExprIdent: return emit(scope, cast<IdentExpr>(ast));

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = scope.slots[ident];
        auto *rhs = emit(scope, stmt.rhs);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(scope, expr.expr);
    }

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(scope, expr.lhs);
        auto *rhs = emit(scope, expr.rhs);

        switch (expr.op) {
        case tok_add: return ir.CreateCall(mod.llvmPy_add(), { lhs, rhs });
        default: return nullptr;
        }
    }

    case ASTType::ExprCall: return emit(scope, cast<CallExpr>(ast));

    case ASTType::ExprLambda: {
        auto &lambda = cast<LambdaExpr>(ast);
        auto *func = createFunction(
                "lambda",
                scope,
                { new ReturnStmt(lambda.expr) });
        return ir.CreateCall(
                mod.llvmPy_func(),
                { func->getInnerFramePtr(),
                  &func->getFunction() });
    }

    default:
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, CallExpr const &call)
{
    RTModule &mod = scope.getModule();

    llvm::Value *lhs = emit(scope, call.lhs);
    lhs->setName(tags.FuncObj);
    vector<llvm::Value *> args;
    args.push_back(nullptr); // Placeholder for the callFrame.
    int argCount = 0;

    for (auto *arg : call.args) {
        llvm::Value *value = emit(scope, *arg);
        value->setName(tags.Arg);
        args.push_back(value);
        argCount++;
    }

    // Call frame pointer.
    llvm::AllocaInst *callFrame = ir.CreateAlloca(
            types.FrameNPtr, 0, tags.CallFrame);

    // Count of positional arguments.
    llvm::Value *np = llvm::ConstantInt::get(types.PyIntValue, argCount);

    llvm::CallInst *inst = ir.CreateCall(
            mod.llvmPy_fchk(),
            { callFrame, lhs, np },
            tags.FuncPtr);

    args[0] = callFrame;

    if (argCount > 1) {
        cerr << "Cannot call function with more than 1 arguments ("
             << argCount << " provided)" << endl;
        exit(1);
    }

    return ir.CreateCall(inst, args, tags.RetVal);
}

llvm::Value *
Emitter::emit(RTScope &scope, IntLitExpr const &expr)
{
    RTModule &mod = scope.getModule();

    llvm::ConstantInt *value =
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    static_cast<uint64_t>(expr.value));

    return ir.CreateCall(mod.llvmPy_int(), {value});
}

llvm::Value *
Emitter::emit(RTScope &scope, IdentExpr const &ident)
{
    RTModule &mod = scope.getModule();

    if (ident.name == "None") {
        return ir.CreateCall(mod.llvmPy_none(), {});
    }

    llvm::Value *slot = scope.slots[ident.name];

    return ir.CreateLoad(slot);
}

RTFunc *
Emitter::createFunction(
        std::string const &name,
        RTScope &outerScope,
        std::vector<Stmt *> const &stmts)
{
    RTModule &mod = outerScope.getModule();

    RTScope *innerScope = outerScope.createDerived();
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();
    vector<llvm::Type *> argTypes;
    argTypes.push_back(types.FrameNPtr);

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            types.Ptr,
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    name,
                    &mod.getModule());

    // Name the arguments.
    int iArg = 0;
    llvm::Value *outerFrameArg = nullptr;
    for (auto &arg : func->args()) {
        if (iArg == 0) {
            arg.setName(tags.OuterFrame);
            outerFrameArg = &arg;
        }

        ++iArg;
    }

    if (!outerFrameArg) {
        throw "Missing outerFrameArg!";
    }

    // Create module body.
    llvm::BasicBlock::Create(ctx, "", func);
    llvm::BasicBlock *init = &func->getEntryBlock();
    llvm::BasicBlock *prog = &func->back();

    int slotCount = 0;

    // Find the number of slots to allocate in the frame.
    for (auto *stmt : stmts) {
        if (stmt->getType() == ASTType::StmtAssign) {
            slotCount++;
        }
    }

    ir.SetInsertPoint(init);

    // Generate the frame.
    llvm::StructType *innerFrameType = types.getFrameN(slotCount);
    llvm::AllocaInst *innerFrameAlloca = ir.CreateAlloca(
            innerFrameType, 0, tags.InnerFrame);

    // Store the frame's self-pointer.
    llvm::Value *frameSelfPtrGEP = ir.CreateGEP(
            innerFrameType,
            innerFrameAlloca,
            { types.getInt64(0),
              types.getInt32(0) });
    ir.CreateStore(innerFrameAlloca, frameSelfPtrGEP);

    // Store the frame's outer pointer.
    llvm::Value *frameOuterPtrGEP = ir.CreateGEP(
            innerFrameType,
            innerFrameAlloca,
            { types.getInt64(0),
              types.getInt32(1) });
    ir.CreateStore(outerFrameArg, frameOuterPtrGEP);

    // Zero-initialise the contents of assign statements. This will act
    // as a sentinel to detect use before set.
    int iSlot = 0;
    for (auto *stmt : stmts) {
        if (stmt->getType() == ASTType::StmtAssign) {
            ir.SetInsertPoint(init);
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            llvm::Value *assignGEP = ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(2),
                      types.getInt64(iSlot) });
            ir.CreateStore(llvm::Constant::getNullValue(types.Ptr), assignGEP);
            iSlot++;

            // TODO: This would be invalidated if the pointer changes
            // TODO: (i.e. if the callee's chain ends up lifting the frame
            // TODO: to heap).
            innerScope->slots[ident] = assignGEP;
        }
    }

    bool lastIsRet = false;

    for (auto *stmt : stmts) {
        ir.SetInsertPoint(prog);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            llvm::Value *value = emit(*innerScope, ret.expr);
            ir.CreateRet(value);
            lastIsRet = true;
        } else {
            emit(*innerScope, *stmt);
            lastIsRet = false;
        }
    }

    if (!lastIsRet) {
        // Return None if no explicit return.
        ir.CreateRet(llvm::ConstantPointerNull::get(types.Ptr));
    }

    llvm::verifyFunction(*func);
    ir.SetInsertPoint(insertPoint);

    RTFunc *rtFunc = new RTFunc(
            *func,
            *innerScope,
            outerFrameArg,
            innerFrameAlloca);

    func->setPrefixData(
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    (uint64_t) rtFunc));

    return rtFunc;
}
