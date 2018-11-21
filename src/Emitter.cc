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
    string Arg = "arg";
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
        argSlots.push_back(value);
        argCount++;
    }

    // Call frame pointer.
    auto *callFrameAlloca = ir.CreateAlloca(types.FramePtr, 0, tags.CallFrame);

    // Count of positional arguments.
    llvm::Value *np = llvm::ConstantInt::get(types.PyIntValue, argCount);

    llvm::CallInst *inst = ir.CreateCall(
            mod.llvmPy_fchk(),
            { callFrameAlloca, lhs, np },
            tags.FuncPtr);

    argSlots[0] = callFrameAlloca;

    llvm::Value *funcBitCast = ir.CreateBitCast(
            inst,
            types.getPtr(types.getOpaqueFunc(argCount)));

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

    auto *gep = findLexicalSlotGEP(name, scope, scope.getInnerFramePtrPtr());
    return ir.CreateLoad(gep);
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
                    types.FramePtr);

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

    RTScope *innerScope = outerScope.createDerived();

    if (outerScope.getInnerFramePtrPtr()) {
        innerScope->setOuterFrameType(outerScope.getInnerFrameType());
    } else {
        innerScope->setOuterFrameType(types.Frame);
    }

    std::set<std::string> slotNames;
    size_t argCount = 0;

    for (auto &argName : args) {
        if (!slotNames.count(argName)) {
            slotNames.insert(argName);
            argCount += 1;
        }
    }

    gatherSlotNames(stmt, slotNames);

    innerScope->setInnerFrameType(
            types.getFuncFrame(
                    name,
                    innerScope->getOuterFrameType(),
                    slotNames.size()));

    llvm::Function *function =
            llvm::Function::Create(
                    types.getFunc(
                            name,
                            innerScope->getOuterFrameType(),
                            argCount),
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
            arg.setName(tags.Arg + "." + *argNames);
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

    // Generate the frame for variables to be potentially lifted onto the
    // heap by a closure.

    auto *frameAlloca = ir.CreateAlloca(
            innerScope->getInnerFrameType(),
            nullptr,
            tags.InnerFrame);

    innerScope->setInnerFramePtr(frameAlloca);

    auto *frameSelfPtrPtr = ir.CreateGEP(
            frameAlloca,
            { types.getInt64(0),
              types.getInt32(Frame::SelfIndex) });

    innerScope->setInnerFramePtrPtr(frameSelfPtrPtr);

    auto *frameOuterPtrPtr = ir.CreateGEP(
            frameAlloca,
            { types.getInt64(0),
              types.getInt32(Frame::OuterIndex) });

    ir.CreateStore(frameAlloca, frameSelfPtrPtr);

    auto *outerFramePtr = ir.CreateLoad(
            outerFramePtrPtrArg, tags.OuterFrame);

    ir.CreateStore(outerFramePtr, frameOuterPtrPtr);

    // TODO: Zero-initialize slots with llvm.memset or something.

    function->setPrefixData(
            types.getInt64(reinterpret_cast<int64_t>(innerScope)));

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

            auto *argVarPtr = ir.CreateGEP(
                    frameAlloca,
                    { types.getInt64(0),
                      types.getInt32(Frame::VarsIndex),
                      types.getInt64(slotIndex) },
                    tags.Var + "_" + ident);

            ir.CreateStore(&arg, argVarPtr);
        }

        iArg++;
    }

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

    size_t condIndex = scope.getNextCondStmtIndex();
    std::string suffix = "." + std::to_string(condIndex);

    auto *ifBB = llvm::BasicBlock::Create(ctx, tags.If + suffix);
    auto *thenBB = llvm::BasicBlock::Create(ctx, tags.Then + suffix);
    auto *elseBB = llvm::BasicBlock::Create(ctx, tags.Else + suffix);
    auto *endifBB = llvm::BasicBlock::Create(ctx, tags.Endif + suffix);

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
        // slotGEP has to come after emission, as something like a lambda
        // may invalidate the existing stack pointer.
        auto *value = emit(scope, assign->getValue());
        auto *slotGEP = findLexicalSlotGEP(assign->getName(), scope);
        ir.CreateStore(value, slotGEP);
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

    auto whileIndex = scope.getNextWhileStmtIndex();
    std::string suffix = "." + std::to_string(whileIndex);

    auto *condBB = llvm::BasicBlock::Create(ctx, tags.While + suffix);
    auto *loopBB = llvm::BasicBlock::Create(ctx, tags.Loop + suffix);
    auto *endwhileBB = llvm::BasicBlock::Create(ctx, tags.Endwhile + suffix);

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
    assert(loop && loop->cond);
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
Emitter::emitDefStmt(
        llvm::Function &outer,
        RTScope &scope,
        DefStmt const &def)
{
    RTModule &mod = scope.getModule();

    // TODO: XXX?
    auto *insertBlock = ir.GetInsertBlock();

    auto *function = createFunction(
            def.getName(),
            scope,
            def.getBody(),
            def.args());

    ir.SetInsertPoint(insertBlock);

    llvm::Value *innerFramePtrBitCast =
            ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    types.FramePtr);

    llvm::Value *functionPtrBitCast =
            ir.CreateBitCast(
                    function,
                    types.i8Ptr);

    auto *value = ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });

    auto *gep = findLexicalSlotGEP(def.getName(), scope);

    ir.CreateStore(value, gep);
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

llvm::Value *
Emitter::findLexicalSlotGEP(
        std::string const &name,
        RTScope &scope,
        llvm::Value *framePtrPtr)
{
    if (!framePtrPtr) {
        framePtrPtr = scope.getInnerFramePtrPtr();
    }

    if (scope.hasSlot(name)) {

        auto index = static_cast<int64_t>(scope.getSlotIndex(name));

        assert(index >= 0); // Later -1 could mean it's not a frame value.

        auto *framePtr = ir.CreateLoad(framePtrPtr);

        auto *slotGEP = ir.CreateGEP(
                framePtr,
                { types.getInt64(0),
                  types.getInt32(Frame::VarsIndex),
                  types.getInt64(index) },
                tags.Var + "." + name);

        return slotGEP;

    } else if (scope.hasParent()) {

        auto *framePtr = ir.CreateLoad(framePtrPtr);

        auto *outerFramePtrPtr = ir.CreateGEP(
                framePtr,
                { types.getInt64(0),
                  types.getInt32(Frame::OuterIndex) });

        auto *result = findLexicalSlotGEP(
                name,
                (RTScope &) scope.getParent(), // TODO
                outerFramePtrPtr);

        if (!result) {
            // Clean up the unused instructions.
            // TODO: Does it leak memory?
            framePtr->eraseFromParent();
            outerFramePtrPtr->deleteValue();
        }

        return result;

    } else {
        assert(false && "Slot not found");
        return nullptr;
    }
}
