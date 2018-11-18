#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/Token.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <llvmPy/Support/iterator_range.h>
#include <iostream>
#include <map>
#include <stdlib.h>
using namespace llvmPy;
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
    string If = "if";
    string Then = "then";
    string Else = "else";
    string Endif = "endif";
    string While = "while";
    string Loop = "loop";
    string Endwhile = "endwhile";
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
        Stmt const &stmt)
{
    auto *module = new llvm::Module(name, ctx);
    module->setDataLayout(dl);
    RTModule *rtModule = new RTModule(name, module, types);

    createFunction(
            "__body__",
            rtModule->getScope(),
            stmt,
            empty_range<std::string const *>());

    llvm::verifyModule(*module);
    return rtModule;
}

RTModule *
Emitter::createModule(std::string const &name)
{
    auto stmt = std::make_unique<CompoundStmt>();
    return createModule(name, *stmt);
}

llvm::Value *
Emitter::emit(RTScope &scope, Expr const &expr)
{
    assert(&expr);

    if (auto *intLit = expr.cast<IntegerExpr>()) {
        return emit(scope, *intLit);
    } else if (auto *ident = expr.cast<IdentExpr>()) {
        return emit(scope, *ident);
    } else if (auto *call = expr.cast<CallExpr>()) {
        return emit(scope, *call);
    } else if (auto *lambda = expr.cast<LambdaExpr>()) {
        return emit(scope, *lambda);
    } else if (auto *strLit = expr.cast<StringExpr>()) {
        return emit(scope, *strLit);
    } else if (auto *unary = expr.cast<UnaryExpr>()) {
        return emit(scope, *unary);
    } else if (auto *binop = expr.cast<BinaryExpr>()) {
        return emit(scope, *binop);
    } else {
        assert(false && "Unrecognised AST");
        return nullptr;
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, CallExpr const &call)
{
    RTModule &mod = scope.getModule();
    auto &callee = call.getCallee();
    auto &args = call.getArguments();

    if (auto *lhsIdent = callee.cast<IdentExpr>()) {
        if (lhsIdent->getName() == "print") {
            llvm::Value *arg = emit(scope, *args[0]);
            return ir.CreateCall(mod.llvmPy_print(), { arg });
        } else if (lhsIdent->getName() == "len") {
            llvm::Value *arg = emit(scope, *args[0]);
            return ir.CreateCall(mod.llvmPy_len(), { arg });
        } else if (lhsIdent->getName() == "str") {
            llvm::Value *arg = emit(scope, *args[0]);
            return ir.CreateCall(mod.llvmPy_str(), { arg });
        } else if (lhsIdent->getName() == "int") {
            llvm::Value *arg = emit(scope, *args[0]);
            return ir.CreateCall(mod.llvmPy_int(), { arg });
        } else if (lhsIdent->getName() == "bool") {
            llvm::Value *arg = emit(scope, *args[0]);
            return ir.CreateCall(mod.llvmPy_bool(), { arg });
        }
    }

    llvm::Value *lhs = emit(scope, callee);
    lhs->setName(tags.FuncObj);
    vector<llvm::Value *> argSlots;
    argSlots.push_back(nullptr); // Placeholder for the callFrame.
    int argCount = 0;

    for (auto &arg : args) {
        llvm::Value *value = emit(scope, *arg);
        value->setName(tags.Arg);
        argSlots.push_back(value);
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

    argSlots[0] = callFrame;

    llvm::Value *funcBitCast = ir.CreateBitCast(
            inst,
            llvm::PointerType::getUnqual(types.getFuncN(argCount)));

    return ir.CreateCall(funcBitCast, argSlots, tags.RetVal);
}

llvm::Value *
Emitter::emit(RTScope &scope, IntegerExpr const &expr)
{
    RTModule &mod = scope.getModule();
    auto *global = mod.llvmPy_PyInt(expr.getValue());
    return ir.CreateLoad(global, global->getName());
}

llvm::Value *
Emitter::emit(RTScope &scope, IdentExpr const &ident)
{
    RTModule &mod = scope.getModule();

    auto const &name = ident.getName();

    if (name == "None") {
        return mod.llvmPy_None();
    } else if (name == "True") {
        return mod.llvmPy_True();
    } else if (name == "False") {
        return mod.llvmPy_False();
    }

    if (scope.hasSlot(name)) {
        auto *slot = scope.getSlotValue(name);
        return ir.CreateLoad(slot);
    } else if (scope.getParent().hasSlot(name)) {
        auto slotIndex = scope.getParent().getSlotIndex(ident.getName());

        llvm::Value *outerFramePtr = ir.CreateLoad(scope.getOuterFramePtr());

        llvm::Value *outerFrameSlotGEP = ir.CreateGEP(
                outerFramePtr,
                { types.getInt64(0),
                  types.getInt32(2),
                  types.getInt64(slotIndex) });

        llvm::Value *outerSlot = ir.CreateLoad(outerFrameSlotGEP);

        return outerSlot;
    } else {
        cerr << "Slot " << ident.getName() << " not found!" << endl;
        exit(127);
    }
}

llvm::Value *
Emitter::emit(RTScope &scope, LambdaExpr const &lambda)
{
    RTModule &mod = scope.getModule();

    ReturnStmt returnStmt(lambda.getExprPtr());

    // TODO: XXX?
    auto *insertBlock = ir.GetInsertBlock();

    auto *function = createFunction(
            tags.Lambda,
            scope,
            returnStmt,
            lambda.args());

    ir.SetInsertPoint(insertBlock);

    llvm::Value *innerFramePtrBitCast =
            ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    types.FrameNPtr);

    llvm::Value *functionPtrBitCast =
            ir.CreateBitCast(
                    function,
                    types.i8Ptr);

    return ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });
}

llvm::Value *
Emitter::emit(RTScope &scope, StringExpr const &lit)
{
    RTModule &mod = scope.getModule();
    auto *global = mod.llvmPy_PyStr(lit.getValue());
    return ir.CreateLoad(global, global->getName());
}

llvm::Value *
Emitter::emit(RTScope &scope, BinaryExpr const &expr)
{
    RTModule &mod = scope.getModule();
    auto *lhs = emit(scope, expr.getLeftOperand());
    auto *rhs = emit(scope, expr.getRightOperand());

    llvm::Value *f;

    switch (expr.getOperator()) {
    case tok_add: f = mod.llvmPy_add(); break;
    case tok_sub: f = mod.llvmPy_sub(); break;
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

llvm::Function *
Emitter::createFunction(
        std::string const &name,
        RTScope &outerScope,
        Stmt const &stmt,
        ArgNamesIter const &args)
{
    RTModule &mod = outerScope.getModule();

    // Names of slots in the frame.
    std::set<std::string> slotNames;

    vector<llvm::Type *> argTypes;

    if (outerScope.getInnerFramePtr()) {
        llvm::Value *outerFramePtr = outerScope.getInnerFramePtr();
        argTypes.push_back(
                llvm::PointerType::getUnqual(outerFramePtr->getType()));
    } else {
        argTypes.push_back(types.FrameNPtrPtr);
    }

    for (auto &argName : args) {
        if (!slotNames.count(argName)) {
            slotNames.insert(argName);
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

    llvm::Value *outerFramePtrPtrArg = nullptr;

    int iArg = 0;
    auto argNames = args.begin();

    for (auto &arg : function->args()) {
        if (iArg == 0) {
            arg.setName(tags.OuterFramePtr);
            outerFramePtrPtrArg = &arg;
        } else {
            arg.setName("arg_" + *argNames);
            ++argNames;
        }

        ++iArg;
    }

    if (!outerFramePtrPtrArg) {
        throw "Missing outerFramePtrPtrArg!";
    }

    // Create function body.
    auto *entry = llvm::BasicBlock::Create(ctx, "", function);
    ir.SetInsertPoint(entry);

    gatherSlotNames(stmt, slotNames);

    // Dereference the outer frame.
    llvm::Value *outerFramePtr =
            ir.CreateLoad(outerFramePtrPtrArg, tags.OuterFrame);

    // Generate the frame.
    llvm::StructType *innerFrameType = types.getFrameN(slotNames.size());
    llvm::AllocaInst *innerFrameAlloca = ir.CreateAlloca(
            innerFrameType, nullptr, tags.InnerFrame);

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

    for (auto const &slotName : slotNames) {
        innerScope->declareSlot(slotName);
    }

    // Copy-initialise the contents of arguments.
    iArg = 0;
    argNames = args.begin();

    for (auto &arg : function->args()) {
        if (iArg > 0) {
            auto ident = *argNames;
            ++argNames;
            assert(innerScope->hasSlot(ident));
            auto slotIndex = innerScope->getSlotIndex(ident);

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
            innerScope->setSlotValue(ident, assignGEP);
        }

        iArg++;
    }

    // Zero-initialise the contents of assign statements. This will act
    // as a sentinel to detect use before set.

    zeroInitialiseSlots(
            stmt,
            *innerScope,
            entry,
            innerFrameType,
            innerFrameAlloca);

    // Successive statements may leave the emitter at a different insert point.

    ir.SetInsertPoint(entry);

    emitStatement(*function, *innerScope, stmt, nullptr);

    if (!lastInstructionWasTerminator()) {
        ir.CreateRet(mod.llvmPy_None());
    }

    llvm::verifyFunction(*function);

    return function;
}

void
Emitter::emitCondStmt(
        llvm::Function &function,
        RTScope &scope,
        ConditionalStmt const &cond,
        Loop const *loop)
{
    RTModule &mod = scope.getModule();

    // If the BBs were immediately linked, the function would end up with
    // a non-linear BBs as any compound statement might itself create more.

    auto *ifBB = llvm::BasicBlock::Create(ctx, tags.If);
    auto *thenBB = llvm::BasicBlock::Create(ctx, tags.Then);
    auto *elseBB = llvm::BasicBlock::Create(ctx, tags.Else);
    auto *endifBB = llvm::BasicBlock::Create(ctx, tags.Endif);

    ir.CreateBr(ifBB);
    ifBB->insertInto(&function);
    ir.SetInsertPoint(ifBB);
    auto *ifExprValue = emit(scope, cond.getCondition());
    auto *truthyValue = ir.CreateCall(mod.llvmPy_truthy(), { ifExprValue });
    ir.CreateCondBr(truthyValue, thenBB, elseBB);

    thenBB->insertInto(&function);
    ir.SetInsertPoint(thenBB);
    emitStatement(function, scope, cond.getThenBranch(), loop);
    if (!lastInstructionWasTerminator()) {
        ir.CreateBr(endifBB);
    }

    elseBB->insertInto(&function);
    ir.SetInsertPoint(elseBB);
    emitStatement(function, scope, cond.getElseBranch(), loop);
    if (!lastInstructionWasTerminator()) {
        ir.CreateBr(endifBB);
    }

    endifBB->insertInto(&function);
    ir.SetInsertPoint(endifBB);
}

void
Emitter::emitStatement(
        llvm::Function &function,
        RTScope &scope,
        Stmt const &stmt,
        Loop const *loop)
{
    if (lastInstructionWasTerminator()) {
        // Any remaining statements would be unreachable code.
        return;
    }

    if (auto *compound = stmt.cast<CompoundStmt>()) {
        for (auto const &innerStmt : compound->getStatements()) {
            emitStatement(function, scope, *innerStmt, loop);
        }
    } else if (auto *expr = stmt.cast<ExprStmt>()) {
        emit(scope, expr->getExpr());
    } else if (auto *ret = stmt.cast<ReturnStmt>()) {
        auto *value = emit(scope, ret->getExpr());
        ir.CreateRet(value);
    } else if (auto *def = stmt.cast<DefStmt>()) {
        emitDefStmt(function, scope, *def);
    } else if (stmt.isa<PassStmt>()) {
        // Ignore.
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        emitCondStmt(function, scope, *cond, loop);
    } else if (auto *assign = stmt.cast<AssignStmt>()) {
        auto *slot = scope.getSlotValue(assign->getName());
        auto *value = emit(scope, assign->getValue());
        ir.CreateStore(value, slot);
    } else if (auto *while_ = stmt.cast<WhileStmt>()) {
        emitWhileStmt(function, scope, *while_);
    } else if (stmt.isa<BreakStmt>()) {
        emitBreakStmt(loop);
    } else if (stmt.isa<ContinueStmt>()) {
        emitContinueStmt(loop);
    } else {
        assert(false);
    }
}

void
Emitter::emitWhileStmt(
        llvm::Function &function,
        RTScope &scope,
        WhileStmt const &stmt)
{
    RTModule &mod = scope.getModule();

    auto *condBB = llvm::BasicBlock::Create(ctx, tags.While);
    auto *loopBB = llvm::BasicBlock::Create(ctx, tags.Loop);
    auto *endwhileBB = llvm::BasicBlock::Create(ctx, tags.Endwhile);

    Loop loop = { .cond = condBB, .end = endwhileBB };

    ir.CreateBr(condBB);

    condBB->insertInto(&function);
    ir.SetInsertPoint(condBB);
    auto *condExprValue = emit(scope, stmt.getCondition());
    auto *truthyValue = ir.CreateCall(mod.llvmPy_truthy(), { condExprValue });
    ir.CreateCondBr(truthyValue, loopBB, endwhileBB);

    loopBB->insertInto(&function);
    ir.SetInsertPoint(loopBB);
    emitStatement(function, scope, stmt.getBody(), &loop);
    if (!lastInstructionWasTerminator()) {
        ir.CreateBr(condBB);
    }

    endwhileBB->insertInto(&function);
    ir.SetInsertPoint(endwhileBB);
}

void
Emitter::emitBreakStmt(Emitter::Loop const *loop)
{
    assert(loop && loop->end);
    ir.CreateBr(loop->end);
}

void
Emitter::emitContinueStmt(Emitter::Loop const *loop)
{
    assert(loop);
    ir.CreateBr(loop->cond);
}

bool
Emitter::lastInstructionWasTerminator() const
{
    return ir.GetInsertBlock()->getTerminator() != nullptr;
}

void
Emitter::gatherSlotNames(Stmt const &stmt, std::set<std::string> &names)
{
    if (auto *assign = stmt.cast<AssignStmt>()) {
        names.insert(assign->getName());
    } else if (auto *def = stmt.cast<DefStmt>()) {
        names.insert(def->getName());
    } else if (auto *compound = stmt.cast<CompoundStmt>()) {
        for (auto const &stmt_ : compound->getStatements()) {
            gatherSlotNames(*stmt_, names);
        }
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        gatherSlotNames(cond->getThenBranch(), names);
        gatherSlotNames(cond->getElseBranch(), names);
    } else if (auto *loop = stmt.cast<WhileStmt>()) {
        gatherSlotNames(loop->getBody(), names);
    }
}

void
Emitter::zeroInitialiseSlots(
        Stmt const &body,
        RTScope &scope,
        llvm::BasicBlock *insertPoint,
        llvm::Type *frameType,
        llvm::Value *frameAlloca)
{
    ir.SetInsertPoint(insertPoint);
    if (body.isa<ExprStmt>()
        || body.isa<PassStmt>()
        || body.isa<BreakStmt>()
        || body.isa<ContinueStmt>()
        || body.isa<ReturnStmt>()) {
        // Ignore.
    } else if (auto *assign = body.cast<AssignStmt>()) {
        zeroInitialiseSlot(assign->getName(), scope, frameType, frameAlloca);
    } else if (auto *def = body.cast<DefStmt>()) {
        zeroInitialiseSlot(def->getName(), scope, frameType, frameAlloca);
    } else if (auto *compound = body.cast<CompoundStmt>()) {
        for (auto const &stmt : compound->getStatements()) {
            zeroInitialiseSlots(
                    *stmt, scope, insertPoint, frameType, frameAlloca);
        }
    } else if (auto *loop = body.cast<WhileStmt>()) {
        zeroInitialiseSlots(
                loop->getBody(),
                scope, insertPoint, frameType, frameAlloca);
    } else if (auto *cond = body.cast<ConditionalStmt>()) {
        zeroInitialiseSlots(
                cond->getThenBranch(),
                scope, insertPoint, frameType, frameAlloca);
        zeroInitialiseSlots(
                cond->getElseBranch(),
                scope, insertPoint, frameType, frameAlloca);
    } else {
        assert(false && "Unhandled statement type");
    }
}

void
Emitter::zeroInitialiseSlot(
        std::string const &name,
        RTScope &scope,
        llvm::Type *frameType,
        llvm::Value *frameAlloca)
{
    if (scope.getSlotValue(name)) {
        // Slot already zero-initialised.
        return;
    }

    auto index = scope.getSlotIndex(name);

    auto *assignGEP = ir.CreateGEP(
            frameType,
            frameAlloca,
            { types.getInt64(0),
              types.getInt32(2),
              types.getInt64(index) },
            tags.Var + "_" + name);

    ir.CreateStore(llvm::Constant::getNullValue(types.Ptr), assignGEP);

    // TODO: This would be invalidated if the pointer changes
    // TODO: (i.e. if the callee's chain ends up lifting the frame
    // TODO: to heap).
    scope.setSlotValue(name, assignGEP);
}

void
Emitter::emitDefStmt(
        llvm::Function &outer,
        RTScope &scope,
        DefStmt const &def)
{
    RTModule &mod = scope.getModule();

    // TODO: XXX?
    auto *insertBlock = ir.GetInsertBlock();

    auto *function = createFunction(
            tags.Def + "_" + def.getName(),
            scope,
            def.getBody(),
            def.args());

    ir.SetInsertPoint(insertBlock);

    llvm::Value *innerFramePtrBitCast =
            ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    types.FrameNPtr);

    llvm::Value *functionPtrBitCast =
            ir.CreateBitCast(
                    function,
                    types.i8Ptr);

    auto *value = ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });

    ir.CreateStore(value, scope.getSlotValue(def.getName()));
}

llvm::Value *
Emitter::emit(RTScope &scope, UnaryExpr const &unary)
{
    if (auto *integer = unary.getExpr().cast<IntegerExpr>()) {
        int64_t sign = 1;

        switch (unary.getOperator()) {
        case tok_sub:
            sign = -1;
        case tok_add: {
            IntegerExpr expr(sign * integer->getValue());
            return emit(scope, expr);
        }

        default:
            break;
        }
    }

    assert(false && "Not Implemented");
}
