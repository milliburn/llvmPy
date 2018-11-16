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
    RTFunc *body = createFunction("__body__", rtModule->getScope(), stmt, {});
    rtModule->setBody(body);
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
Emitter::emit(RTScope &scope, AST const &ast)
{
    if (auto *intLit = ast.cast<IntegerExpr>()) {
        return emit(scope, *intLit);
    } else if (auto *ident = ast.cast<IdentExpr>()) {
        return emit(scope, *ident);
    } else if (auto *call = ast.cast<CallExpr>()) {
        return emit(scope, *call);
    } else if (auto *lambda = ast.cast<LambdaExpr>()) {
        return emit(scope, *lambda);
    } else if (auto *def = ast.cast<DefStmt>()) {
        return emit(scope, *def);
    } else if (auto *strLit = ast.cast<StringExpr>()) {
        return emit(scope, *strLit);
    } else if (auto *binop = ast.cast<BinaryExpr>()) {
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

    llvm::ConstantInt *value =
            llvm::ConstantInt::get(
                    types.PyIntValue,
                    static_cast<uint64_t>(expr.getValue()));

    return ir.CreateCall(mod.llvmPy_int(), { value });
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

    auto stmt = std::make_unique<ReturnStmt>(lambda.expr);

    RTFunc *func =
            createFunction(
                    tags.Lambda,
                    scope,
                    *stmt,
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
                    tags.Def + "_" + def.getName(),
                    scope,
                    def.getBody(),
                    def.getArguments());

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
Emitter::emit(RTScope &scope, StringExpr const &lit)
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

RTFunc *
Emitter::createFunction(
        std::string const &name,
        RTScope &outerScope,
        Stmt const &stmt_,
        std::vector<std::string const> const &args)
{
    // TODO: Fix this temporary adapter.
    std::vector<Stmt const *> stmts;
    if (auto *compound = stmt_.cast<CompoundStmt>()) {
        for (auto const &stmt : compound->getStatements()) {
            stmts.push_back(stmt.get());
        }
    } else {
        stmts.push_back(&stmt_);
    }

    RTModule &mod = outerScope.getModule();

    // Names of slots in the frame.
    std::set<std::string const> slotNames;

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
    llvm::BasicBlock *prog = init;

    gatherSlotNames(stmt_, slotNames);

    ir.SetInsertPoint(init);

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
    for (auto &arg : function->args()) {
        if (iArg > 0) {
            auto ident = args[iArg - 1];
            ir.SetInsertPoint(init);
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
            stmt_,
            *innerScope,
            init,
            innerFrameType,
            innerFrameAlloca);

    // Successive statements may leave the emitter at a different insert point.

    ir.SetInsertPoint(prog);

    for (auto *stmt : stmts) {
        emitStatement(*function, *innerScope, *stmt, nullptr);
    }

    if (!lastInstructionWasTerminator()) {
        ir.CreateRet(mod.llvmPy_None());
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

    auto *thenBB = llvm::BasicBlock::Create(ctx, tags.Then);
    auto *elseBB = llvm::BasicBlock::Create(ctx, tags.Else);
    auto *endifBB = llvm::BasicBlock::Create(ctx, tags.Endif);

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
        // No way for control to flow here.
        return;
    }

    if (auto *compound = stmt.cast<CompoundStmt>()) {
        for (auto const &innerStmt : compound->getStatements()) {
            emitStatement(function, scope, *innerStmt, loop);
        }
    } else if (auto *expr = stmt.cast<ExprStmt>()) {
        emit(scope, expr->expr);
    } else if (auto *ret = stmt.cast<ReturnStmt>()) {
        auto *value = emit(scope, ret->expr);
        ir.CreateRet(value);
    } else if (auto *def = stmt.cast<DefStmt>()) {
        auto *value = emit(scope, *def);
        ir.CreateStore(value, scope.getSlotValue(def->getName()));
    } else if (stmt.isa<PassStmt>()) {
        // Ignore.
    } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
        emitCondStmt(function, scope, *cond, loop);
    } else if (auto *assign = stmt.cast<AssignStmt>()) {
        auto *slot = scope.getSlotValue(assign->lhs);
        auto *value = emit(scope, assign->rhs);
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
    assert(loop);
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
    return ir.GetInsertBlock()->back().isTerminator();
}

void
Emitter::gatherSlotNames(Stmt const &stmt, std::set<std::string const> &names)
{
    if (auto *assign = stmt.cast<AssignStmt>()) {
        names.insert(assign->lhs);
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
        zeroInitialiseSlot(assign->lhs, scope, frameType, frameAlloca);
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

