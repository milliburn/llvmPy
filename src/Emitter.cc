#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/SyntaxError.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <map>
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
using std::map;

static struct {
    string FuncObj = "";
    string PosArgCount = "";
    string FuncPtr = "";
    string RetVal = "";
    string Arg = "";
    string OuterFrame = "outer";
    string OuterFramePtr = "outerptr";
    string InnerFrame = "frame";
    string CallFrame = "callframe";
    string Lambda = "lambda";
    string Def = "def";
    string Var = "var";
    string String = "str";
} tags;

Emitter::Emitter(Compiler &c) noexcept
: dl(c.getDataLayout()),
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
    module->setDataLayout(dl);
    RTModule *rtModule = new RTModule(name, module, types);
    RTFunc *body = createFunction("__body__", rtModule->getScope(), stmts, {});
    rtModule->setBody(body);
    llvm::verifyModule(*module);
    return rtModule;
}

llvm::Value *
Emitter::emit(RTScope &scope, AST const &ast)
{
    switch (ast.getType()) {
    case ASTType::ExprIntLit: return emit(scope, cast<IntLitExpr>(ast));
    case ASTType::ExprIdent: return emit(scope, cast<IdentExpr>(ast));
    case ASTType::ExprCall: return emit(scope, cast<CallExpr>(ast));
    case ASTType::ExprLambda: return emit(scope, cast<LambdaExpr>(ast));
    case ASTType::StmtDef: return emit(scope, cast<DefStmt>(ast));
    case ASTType::ExprStrLit: return emit(scope, cast<StrLitExpr>(ast));
    case ASTType::ExprBinary: return emit(scope, cast<BinaryExpr>(ast));

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

    default:
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, CallExpr const &call)
{
    RTModule &mod = scope.getModule();

    if (isa<IdentExpr>(call.lhs)) {
        auto lhsIdent = cast<IdentExpr>(call.lhs);
        if (lhsIdent.name == "print") {
            llvm::Value *arg = emit(scope, *call.args[0]);
            return ir.CreateCall(mod.llvmPy_print(), { arg });
        }
    }

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

    llvm::Value *funcBitCast = ir.CreateBitCast(
            inst,
            llvm::PointerType::getUnqual(types.getFuncN(argCount)));

    return ir.CreateCall(funcBitCast, args, tags.RetVal);
}

llvm::Value *
Emitter::emit(RTScope &scope, IntLitExpr const &expr)
{
    RTModule &mod = scope.getModule();

    llvm::ConstantInt *value =
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    static_cast<uint64_t>(expr.value));

    return ir.CreateCall(mod.llvmPy_int(), { value });
}

llvm::Value *
Emitter::emit(RTScope &scope, IdentExpr const &ident)
{
    RTModule &mod = scope.getModule();

    if (ident.name == "None") {
        return ir.CreateCall(mod.llvmPy_none(), {});
    } else if (ident.name == "True") {

        llvm::ConstantInt *value =
                llvm::ConstantInt::get(
                        types.PyIntValue,
                        static_cast<uint64_t>(1));

        return ir.CreateCall(mod.llvmPy_bool(), { value });
    } else if (ident.name == "False") {
        llvm::ConstantInt *value =
                llvm::ConstantInt::get(
                        types.PyIntValue,
                        static_cast<uint64_t>(0));

        return ir.CreateCall(mod.llvmPy_bool(), { value });
    }

    llvm::Value *slot = scope.slots[ident.name];

    if (slot == nullptr) {
        // May be in an upper scope.
        slot = scope.getParent().slots[ident.name];

        if (!slot) {
            cerr << "Slot " << ident.name << " not found!" << endl;
            exit(127);
        }

        llvm::Value *outerFramePtr = ir.CreateLoad(scope.getOuterFramePtr());

        llvm::Value *outerFrameSlotGEP = ir.CreateGEP(
                outerFramePtr,
                { types.getInt64(0),
                  types.getInt32(2),
                  types.getInt64(0) }); // TODO: True index of slot.

        llvm::Value *outerSlot = ir.CreateLoad(outerFrameSlotGEP);

        return outerSlot;
    }

    return ir.CreateLoad(slot);
}

llvm::Value *
Emitter::emit(RTScope &scope, LambdaExpr const &lambda)
{
    RTModule &mod = scope.getModule();

    RTFunc *func =
            createFunction(
                    tags.Lambda,
                    scope,
                    { new ReturnStmt(lambda.expr) },
                    lambda.args);

    llvm::Value *innerFramePtrBitCast =
            ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    types.FrameNPtr);

    llvm::Value *functionPtrBitCast =
            ir.CreateBitCast(
                    &func->getFunction(),
                    types.i8Ptr);

    return ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });
}

llvm::Value *
Emitter::emit(RTScope &scope, DefStmt const &def)
{
    RTModule &mod = scope.getModule();

    RTFunc *func =
            createFunction(
                    tags.Def + "_" + def.name,
                    scope,
                    def.stmts,
                    def.args);

    llvm::Value *innerFramePtrBitCast =
            ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    types.FrameNPtr);

    llvm::Value *functionPtrBitCast =
            ir.CreateBitCast(
                    &func->getFunction(),
                    types.i8Ptr);

    return ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });
}

llvm::Value *
Emitter::emit(RTScope &scope, StrLitExpr const &lit)
{
    RTModule &mod = scope.getModule();

    llvm::Value *globalString =
            ir.CreateGlobalStringPtr(
                    lit.getValue(),
                    tags.String);

    return ir.CreateCall(mod.llvmPy_str(), { globalString });
}

llvm::Value *
Emitter::emit(RTScope &scope, BinaryExpr const &expr)
{
    RTModule &mod = scope.getModule();
    auto *lhs = emit(scope, expr.lhs);
    auto *rhs = emit(scope, expr.rhs);

    llvm::Value *f;

    switch (expr.op) {
    case tok_add: f = mod.llvmPy_add(); break;
    case tok_mul: f = mod.llvmPy_mul(); break;
    case tok_lt: f = mod.llvmPy_lt(); break;
    case tok_lte: f = mod.llvmPy_le(); break;
    case tok_eq: f = mod.llvmPy_eq(); break;
    case tok_neq: f = mod.llvmPy_ne(); break;
    case tok_gte: f = mod.llvmPy_ge(); break;
    case tok_gt: f = mod.llvmPy_gt(); break;
    default: return nullptr;
    }

    return ir.CreateCall(f, { lhs, rhs });
}

RTFunc *
Emitter::createFunction(
        std::string const &name,
        RTScope &outerScope,
        std::vector<Stmt *> const &stmts,
        std::vector<std::string const> const &args)
{
    RTModule &mod = outerScope.getModule();
    map<string, size_t> slots; // Names and indices of slots in the frame.

    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();
    vector<llvm::Type *> argTypes;

    if (outerScope.getInnerFramePtr()) {
        llvm::Value *outerFramePtr = outerScope.getInnerFramePtr();
        argTypes.push_back(
                llvm::PointerType::getUnqual(outerFramePtr->getType()));
    } else {
        argTypes.push_back(types.FrameNPtrPtr);
    }

    for (auto &argName : args) {
        if (!slots.count(argName)) {
            slots[argName] = slots.size();
            argTypes.push_back(types.Ptr);
        }
    }

    llvm::Function *function =
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
    llvm::Value *outerFramePtrPtrArg = nullptr;
    for (auto &arg : function->args()) {
        if (iArg == 0) {
            arg.setName(tags.OuterFramePtr);
            outerFramePtrPtrArg = &arg;
        } else {
            arg.setName("arg_" + args[iArg - 1]);
        }

        ++iArg;
    }

    if (!outerFramePtrPtrArg) {
        throw "Missing outerFramePtrPtrArg!";
    }

    // Create function body.
    llvm::BasicBlock::Create(ctx, "", function);
    llvm::BasicBlock *init = &function->getEntryBlock();
    llvm::BasicBlock *prog = &function->back();

    // Add slots that originate from assignments.
    for (auto *stmt : stmts) {
        if (stmt->getType() == ASTType::StmtAssign) {
            auto assignStmt = *cast<AssignStmt>(stmt);
            auto ident = assignStmt.lhs;
            if (!slots.count(ident)) {
                slots[ident] = slots.size();
            }
        } else if (stmt->getType() == ASTType::StmtDef) {
            auto defStmt = *cast<DefStmt>(stmt);
            auto ident = defStmt.name;
            if (!slots.count(ident)) {
                slots[ident] = slots.size();
            }
        }
    }

    ir.SetInsertPoint(init);

    // Dereference the outer frame.
    llvm::Value *outerFramePtr =
            ir.CreateLoad(outerFramePtrPtrArg, tags.OuterFrame);

    // Generate the frame.
    llvm::StructType *innerFrameType = types.getFrameN(slots.size());
    llvm::AllocaInst *innerFrameAlloca = ir.CreateAlloca(
            innerFrameType, 0, tags.InnerFrame);

    // Store the frame's self-pointer.
    llvm::Value *frameSelfPtrGEP =
            ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(0) });
    ir.CreateStore(innerFrameAlloca, frameSelfPtrGEP);

    // Store the frame's outer pointer.

    llvm::Value *frameOuterPtrGEP =
            ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(1) });

    llvm::Value *frameOuterPtrGEPBitCast =
            ir.CreateBitCast(
                    frameOuterPtrGEP,
                    llvm::PointerType::getUnqual(
                            outerFramePtr->getType()));

    ir.CreateStore(outerFramePtr, frameOuterPtrGEPBitCast);

    RTScope *innerScope =
            outerScope.createDerived(
                    innerFrameAlloca,
                    outerFramePtrPtrArg);

    // Copy-initialise the contents of arguments.
    iArg = 0;
    for (auto &arg : function->args()) {
        if (iArg > 0) {
            auto ident = args[iArg - 1];
            ir.SetInsertPoint(init);
            assert(slots.count(ident));
            auto slotIndex = slots[ident];
            llvm::Value *assignGEP = ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(2),
                      types.getInt64(slotIndex) },
                    tags.Var + "_" + ident);
            ir.CreateStore(&arg, assignGEP);

            // TODO: This would be invalidated if the pointer changes
            // TODO: (i.e. if the callee's chain ends up lifting the frame
            // TODO: to heap).
            innerScope->slots[ident] = assignGEP;
        }

        iArg++;
    }

    // Zero-initialise the contents of assign statements. This will act
    // as a sentinel to detect use before set.
    for (auto *stmt : stmts) {
        // TODO: Remove code duplication.
        if (stmt->getType() == ASTType::StmtAssign) {
            ir.SetInsertPoint(init);
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            assert(slots.count(ident));
            auto slotIndex = slots[ident];
            llvm::Value *assignGEP = ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(2),
                      types.getInt64(slotIndex) },
                    tags.Var + "_" + ident);
            ir.CreateStore(llvm::Constant::getNullValue(types.Ptr), assignGEP);

            // TODO: This would be invalidated if the pointer changes
            // TODO: (i.e. if the callee's chain ends up lifting the frame
            // TODO: to heap).
            innerScope->slots[ident] = assignGEP;
        } else if (stmt->getType() == ASTType::StmtDef) {
            ir.SetInsertPoint(init);
            auto &def = *cast<DefStmt>(stmt);
            auto &ident = def.name;
            assert(slots.count(ident));
            auto slotIndex = slots[ident];
            llvm::Value *assignGEP = ir.CreateGEP(
                    innerFrameType,
                    innerFrameAlloca,
                    { types.getInt64(0),
                      types.getInt32(2),
                      types.getInt64(slotIndex) },
                    tags.Var + "_" + ident);
            ir.CreateStore(llvm::Constant::getNullValue(types.Ptr), assignGEP);

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
        } else if (isa<DefStmt>(stmt)) {
            auto &def = *cast<DefStmt>(stmt);
            llvm::Value *value = emit(*innerScope, def);
            ir.CreateStore(value, innerScope->slots[def.name]);
            lastIsRet = false;
        } else {
            emit(*innerScope, *stmt);
            lastIsRet = false;
        }
    }

    if (!lastIsRet) {
        // Return None if no explicit return.
        ir.CreateRet(llvm::ConstantPointerNull::get(types.Ptr));
    }

    llvm::verifyFunction(*function);
    ir.SetInsertPoint(insertPoint);

    RTFunc *rtFunc = new RTFunc(*function, *innerScope);

    function->setPrefixData(
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    (uint64_t) rtFunc));

    return rtFunc;
}
