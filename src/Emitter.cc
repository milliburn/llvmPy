#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#pragma GCC diagnostic pop

#include <llvmPy/Emitter.h>
#include <llvmPy/RT.h>
#include <llvmPy/Token.h>
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
    string Name = "name";
} tags;

Emitter::Emitter(Compiler &c) noexcept
: _dl(c.getDataLayout()),
  _ctx(c.getContext()),
  _ir(_ctx),
  _types(_ctx, _dl)
{
}

RTModule *
Emitter::createModule(
        std::string const &name,
        Stmt const &stmt)
{
    auto *module = new llvm::Module(name, _ctx);
    module->setDataLayout(_dl);
    RTModule *rtModule = new RTModule(name, module, _types);

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
    } else if (auto *getattr = expr.cast<GetattrExpr>()) {
        return emit(scope, *getattr, nullptr);
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
            return _ir.CreateCall(mod.llvmPy_print(), { arg });
        } else if (lhsIdent->getName() == "len") {
            llvm::Value *arg = emit(scope, *args[0]);
            return _ir.CreateCall(mod.llvmPy_len(), { arg });
        } else if (lhsIdent->getName() == "str") {
            llvm::Value *arg = emit(scope, *args[0]);
            return _ir.CreateCall(mod.llvmPy_str(), { arg });
        } else if (lhsIdent->getName() == "int") {
            llvm::Value *arg = emit(scope, *args[0]);
            return _ir.CreateCall(mod.llvmPy_int(), { arg });
        } else if (lhsIdent->getName() == "bool") {
            llvm::Value *arg = emit(scope, *args[0]);
            return _ir.CreateCall(mod.llvmPy_bool(), { arg });
        }
    }

    bool isGetattrCall = false;
    llvm::Value *self = nullptr;
    llvm::Value *lhs;

    if (auto *getattr = call.getCallee().cast<GetattrExpr>()) {
        isGetattrCall = true;
        lhs = emit(scope, *getattr, &self);
    } else {
        lhs = emit(scope, callee);
    }

    lhs->setName(tags.FuncObj);
    vector<llvm::Value *> argSlots;
    argSlots.push_back(nullptr); // Placeholder for the callFrame.

    size_t argCount = 0;

    if (isGetattrCall) {
        argSlots.push_back(self);
        argCount += 1;
    }

    for (auto &arg : args) {
        llvm::Value *value = emit(scope, *arg);
        argSlots.push_back(value);
        argCount += 1;
    }

    // Call frame pointer.
    auto *callFrameAlloca = _ir.CreateAlloca(
            _types.FramePtr, nullptr, tags.CallFrame);

    // Count of positional arguments.
    llvm::Value *np = llvm::ConstantInt::get(_types.PyIntValue, argCount);

    llvm::CallInst *inst = _ir.CreateCall(
            mod.llvmPy_fchk(),
            { callFrameAlloca, lhs, np },
            tags.FuncPtr);

    argSlots[0] = callFrameAlloca;

    llvm::Value *funcBitCast = _ir.CreateBitCast(
            inst,
            _types.getPtr(_types.getOpaqueFunc(argCount)));

    return _ir.CreateCall(funcBitCast, argSlots, tags.RetVal);
}

llvm::Value *
Emitter::emit(RTScope &scope, IntegerExpr const &expr)
{
    RTModule &mod = scope.getModule();
    auto *global = mod.llvmPy_PyInt(expr.getValue());
    return _ir.CreateLoad(global, global->getName());
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
    return _ir.CreateLoad(gep);
}

llvm::Value *
Emitter::emit(RTScope &scope, LambdaExpr const &lambda)
{
    RTModule &mod = scope.getModule();

    ReturnStmt returnStmt(lambda.getExprPtr());

    // TODO: XXX?
    auto *insertBlock = _ir.GetInsertBlock();

    auto *function = createFunction(
            tags.Lambda,
            scope,
            returnStmt,
            lambda.args());

    _ir.SetInsertPoint(insertBlock);

    llvm::Value *innerFramePtrBitCast =
            _ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    _types.FramePtr);

    llvm::Value *functionPtrBitCast =
            _ir.CreateBitCast(
                    function,
                    _types.i8Ptr);

    return _ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });
}

llvm::Value *
Emitter::emit(RTScope &scope, StringExpr const &lit)
{
    RTModule &mod = scope.getModule();
    auto *global = mod.llvmPy_PyStr(lit.getValue());
    return _ir.CreateLoad(global, global->getName());
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

    return _ir.CreateCall(f, { lhs, rhs });
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
        innerScope->setOuterFrameType(_types.Frame);
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
            _types.getFuncFrame(
                    name,
                    innerScope->getOuterFrameType(),
                    slotNames.size()));

    llvm::Function *function =
            llvm::Function::Create(
                    _types.getFunc(
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
    auto *entry = llvm::BasicBlock::Create(_ctx, "", function);
    _ir.SetInsertPoint(entry);

    // Generate the frame for variables to be potentially lifted onto the
    // heap by a closure.

    auto *frameAlloca = _ir.CreateAlloca(
            innerScope->getInnerFrameType(),
            nullptr,
            tags.InnerFrame);

    innerScope->setInnerFramePtr(frameAlloca);

    auto *frameSelfPtrPtr = _ir.CreateGEP(
            frameAlloca,
            { _types.getInt64(0),
              _types.getInt32(Frame::SelfIndex) });

    innerScope->setInnerFramePtrPtr(frameSelfPtrPtr);

    auto *frameOuterPtrPtr = _ir.CreateGEP(
            frameAlloca,
            { _types.getInt64(0),
              _types.getInt32(Frame::OuterIndex) });

    _ir.CreateStore(frameAlloca, frameSelfPtrPtr);

    auto *outerFramePtr = _ir.CreateLoad(
            outerFramePtrPtrArg, tags.OuterFrame);

    _ir.CreateStore(outerFramePtr, frameOuterPtrPtr);

    // TODO: Zero-initialize slots with llvm.memset or something.

    function->setPrefixData(
            _types.getInt64(reinterpret_cast<int64_t>(innerScope)));

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

            auto *argVarPtr = _ir.CreateGEP(
                    frameAlloca,
                    { _types.getInt64(0),
                      _types.getInt32(Frame::VarsIndex),
                      _types.getInt64(static_cast<int64_t>(slotIndex)) },
                    tags.Var + "_" + ident);

            _ir.CreateStore(&arg, argVarPtr);
        }

        iArg++;
    }

    _ir.SetInsertPoint(entry);

    emitStatement(*function, *innerScope, stmt, nullptr);

    if (!lastInstructionWasTerminator()) {
        _ir.CreateRet(mod.llvmPy_None());
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

    auto *ifBB = llvm::BasicBlock::Create(_ctx, tags.If + suffix);
    auto *thenBB = llvm::BasicBlock::Create(_ctx, tags.Then + suffix);
    auto *elseBB = llvm::BasicBlock::Create(_ctx, tags.Else + suffix);
    auto *endifBB = llvm::BasicBlock::Create(_ctx, tags.Endif + suffix);

    _ir.CreateBr(ifBB);
    ifBB->insertInto(&function);
    _ir.SetInsertPoint(ifBB);
    auto *ifExprValue = emit(scope, cond.getCondition());
    auto *truthyValue = _ir.CreateCall(mod.llvmPy_truthy(), { ifExprValue });
    _ir.CreateCondBr(truthyValue, thenBB, elseBB);

    thenBB->insertInto(&function);
    _ir.SetInsertPoint(thenBB);
    emitStatement(function, scope, cond.getThenBranch(), loop);
    if (!lastInstructionWasTerminator()) {
        _ir.CreateBr(endifBB);
    }

    elseBB->insertInto(&function);
    _ir.SetInsertPoint(elseBB);
    emitStatement(function, scope, cond.getElseBranch(), loop);
    if (!lastInstructionWasTerminator()) {
        _ir.CreateBr(endifBB);
    }

    endifBB->insertInto(&function);
    _ir.SetInsertPoint(endifBB);
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
        _ir.CreateRet(value);
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
        _ir.CreateStore(value, slotGEP);
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

    auto *condBB = llvm::BasicBlock::Create(_ctx, tags.While + suffix);
    auto *loopBB = llvm::BasicBlock::Create(_ctx, tags.Loop + suffix);
    auto *endwhileBB = llvm::BasicBlock::Create(_ctx, tags.Endwhile + suffix);

    Loop loop = { .cond = condBB, .end = endwhileBB };

    _ir.CreateBr(condBB);

    condBB->insertInto(&function);
    _ir.SetInsertPoint(condBB);
    auto *condExprValue = emit(scope, stmt.getCondition());
    auto *truthyValue = _ir.CreateCall(mod.llvmPy_truthy(), { condExprValue });
    _ir.CreateCondBr(truthyValue, loopBB, endwhileBB);

    loopBB->insertInto(&function);
    _ir.SetInsertPoint(loopBB);
    emitStatement(function, scope, stmt.getBody(), &loop);
    if (!lastInstructionWasTerminator()) {
        _ir.CreateBr(condBB);
    }

    endwhileBB->insertInto(&function);
    _ir.SetInsertPoint(endwhileBB);
}

void
Emitter::emitBreakStmt(Emitter::Loop const *loop)
{
    assert(loop && loop->end);
    _ir.CreateBr(loop->end);
}

void
Emitter::emitContinueStmt(Emitter::Loop const *loop)
{
    assert(loop && loop->cond);
    _ir.CreateBr(loop->cond);
}

bool
Emitter::lastInstructionWasTerminator() const
{
    return _ir.GetInsertBlock()->getTerminator() != nullptr;
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
    auto *insertBlock = _ir.GetInsertBlock();

    auto *function = createFunction(
            def.getName(),
            scope,
            def.getBody(),
            def.args());

    _ir.SetInsertPoint(insertBlock);

    llvm::Value *innerFramePtrBitCast =
            _ir.CreateBitCast(
                    scope.getInnerFramePtr(),
                    _types.FramePtr);

    llvm::Value *functionPtrBitCast =
            _ir.CreateBitCast(
                    function,
                    _types.i8Ptr);

    auto *value = _ir.CreateCall(
            mod.llvmPy_func(),
            { innerFramePtrBitCast,
              functionPtrBitCast });

    auto *gep = findLexicalSlotGEP(def.getName(), scope);

    _ir.CreateStore(value, gep);
}

llvm::Value *
Emitter::emit(RTScope &scope, UnaryExpr const &unary)
{
    if (auto *integer = unary.getExpr().cast<IntegerExpr>()) {
        int64_t sign = 1;

        switch (unary.getOperator()) {
        case tok_sub:
            sign = -1;
            [[clang::fallthrough]];
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

        auto *framePtr = _ir.CreateLoad(framePtrPtr);

        auto *slotGEP = _ir.CreateGEP(
                framePtr,
                { _types.getInt64(0),
                  _types.getInt32(Frame::VarsIndex),
                  _types.getInt64(index) },
                tags.Var + "." + name);

        return slotGEP;

    } else if (scope.hasParent()) {

        auto *framePtr = _ir.CreateLoad(framePtrPtr);

        auto *outerFramePtrPtr = _ir.CreateGEP(
                framePtr,
                { _types.getInt64(0),
                  _types.getInt32(Frame::OuterIndex) });

        auto *result = findLexicalSlotGEP(
                name,
                dynamic_cast<RTScope &>(scope.getParent()),
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

llvm::Value *
Emitter::emit(
        RTScope &scope,
        GetattrExpr const &getattr,
        llvm::Value **objectOut)
{
    auto &module = scope.getModule();
    auto *object = emit(scope, getattr.getObject());
    auto *name = _ir.CreateGlobalStringPtr(
            getattr.getName(),
            tags.Name + "." + getattr.getName());
    auto *value = _ir.CreateCall(
            module.llvmPy_getattr(),
            { object, name });

    if (objectOut) {
        *objectOut = object;
    }

    return value;
}
