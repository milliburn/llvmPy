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

static constexpr int RTAtom_type = 0;
static constexpr int RTAtom_atom = 1;

Emitter::Emitter(Compiler &c) noexcept
: rt(c.getRT()),
  dl(c.getDataLayout()),
  ctx(c.getContext()),
  ir(ctx),
  types(ctx, dl)
{
}

llvm::Value *
Emitter::emitValue(AST const &ast, llvm::Module &mod, RTScope &scope)
{
    switch (ast.getType()) {

        /* Constant expressions */

    case ASTType::ExprDecLit: {
        auto &expr = cast<DecLitExpr>(ast);
        return emitAlloca(RTDecAtom(expr.value), "lit");
    }

    case ASTType::ExprIntLit: {
        // TODO: Proper integers.
        auto &expr = cast<IntLitExpr>(ast);
        return emitAlloca(RTIntAtom(expr.value), "lit");
    }

    case ASTType::ExprBoolLit: {
        auto &expr = cast<BoolLitExpr>(ast);
        return emitAlloca(RTBoolAtom(expr.value), "lit");
    }

    case ASTType::ExprIdent: {
        auto &ident = cast<IdentExpr>(ast);
        auto *value = scope.slots[ident.name];

        if (!value) {
            throw SyntaxError("Slot " + ident.name + " does not exist");
        }

        //return ir.CreateLoad(value, ident.name);
        return value;
    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emitValue(expr.expr, mod, scope);
    }

    default:
        throw EmitterError(
                "Unrecognised ASTType " + to_string(
                        static_cast<int>(ast.getType())));
    }
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

llvm::Value *
Emitter::emit(
        AST const &ast,
        llvm::Module &module,
        RTScope &scope)
{
    switch (ast.getType()) {

    /* Constant expressions */

    case ASTType::ExprDecLit: {
        auto &expr = cast<DecLitExpr>(ast);
        return emitAlloca(RTDecAtom(expr.value), "lit");
    }

    case ASTType::ExprIntLit: {
        // TODO: Proper integers.
        auto &expr = cast<IntLitExpr>(ast);
        return emitAlloca(RTIntAtom(expr.value), "lit");
    }

    case ASTType::ExprBoolLit: {
        auto &expr = cast<BoolLitExpr>(ast);
        return emitAlloca(RTBoolAtom(expr.value), "lit");
    }

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = scope.slots[ident];
        auto *rhs = emit(stmt.rhs, module, scope);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::ExprIdent: {
        auto &ident = cast<IdentExpr>(ast);
        auto *value = scope.slots[ident.name];

        if (!value) {
            throw SyntaxError("Slot " + ident.name + " does not exist");
        }

        //return ir.CreateLoad(value, ident.name);
        return value;
    }

    case ASTType::ExprCall: {
        auto &call = cast<CallExpr>(ast);
        auto *callee = emit(call.lhs, module, scope);
        vector<llvm::Value *> args;
        for (auto arg : call.args) {
            args.push_back(emit(*arg, module, scope));
        }
        return ir.CreateCall(callee, args, "call");
    }

    case ASTType::StmtDef: {
        auto &def = cast<DefStmt>(ast);
        auto *func = emitFunction(
                def.name,
                def.args,
                def.stmts,
                module,
                &scope);
        // TODO: Assign to slot.
        return func->ir;
    }

    case ASTType::ExprLambda: {
        auto &lambda = cast<LambdaExpr>(ast);
        auto *func = emitFunction(
                "lambda",
                lambda.args,
                { new ReturnStmt(lambda.expr) },
                module,
                &scope);
        return func->ir;
    }

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(expr.lhs, module, scope);
        auto *rhs = emit(expr.rhs, module, scope);
        auto *rv = ir.CreateAlloca(types.RTAtom, 0, "rv");

        switch (expr.op) {
        case tok_add:
            ir.CreateCall(
                    module.getOrInsertFunction("lpy_add", types.lpy_add),
                    { rv, lhs, rhs });
            return rv;

        default: return nullptr;
        }

    }

    case ASTType::StmtExpr: {
        auto &expr = cast<ExprStmt>(ast);
        return emit(expr.expr, module, scope);
    }

    default:
        throw EmitterError(
                "Unrecognised ASTType " + to_string(
                        static_cast<int>(ast.getType())));
    }
}

RTModuleObj *
Emitter::emitModule(
        vector<Stmt *> const &stmts,
        string const &name)
{
    llvm::Module *ir = new llvm::Module(name, ctx);

    RTFuncObj *func = emitFunction(
            "__body__",
            {},
            stmts,
            *ir,
            nullptr);

    RTModuleObj *rtmodule = new RTModuleObj(
            name,
            func->scope,
            func,
            ir);

    rt.modules.insert(make_pair(name, rtmodule));

    return rtmodule;
}

RTModuleObj *
Emitter::emitModuleNaked(std::vector<Stmt *> const &stmts, std::string &name)
{
    llvm::Module *mod = new llvm::Module(name, ctx);
    RTScope *scope = new RTScope();

    // Store original insert point.
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();

    vector<llvm::Type *> argTypes;

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            llvm::Type::getVoidTy(ctx),
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    "__body__",
                    mod);

    llvm::BasicBlock *bb =
            llvm::BasicBlock::Create(ctx, "", func);

    ir.SetInsertPoint(bb);

    for (auto stmt : stmts) {
        switch (stmt->getType()) {
        case ASTType::StmtAssign: {
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            scope->slots[ident] = emitAlloca(RTNoneAtom(), ident);
            break;
        }

        default: break;
        }
    }

    for (auto stmt : stmts) {
        ir.SetInsertPoint(bb);
        emit(*stmt, *mod, *scope);
    }

    // Restore original insert point.
    ir.SetInsertPoint(insertPoint);

    RTFuncObj *rtFunc = new RTFuncObj("__body__", scope, func);
    RTModuleObj *rtModule = new RTModuleObj(name, rtFunc->scope, rtFunc, mod);
    return rtModule;
}

RTModuleObj *
Emitter::emitModuleNaked(std::vector<Stmt *> const &stmts)
{
    std::string name("");
    return emitModuleNaked(stmts, name);
}

RTFuncObj *
Emitter::emitFunction(
        string const &name,
        vector<string const *> const args,
        vector<Stmt *> const body,
        llvm::Module &module,
        RTScope * const outer)
{
    RTScope &scope = *new RTScope(outer);
    long const irArgCount = 1 + args.size();
    llvm::BasicBlock *insertPoint = ir.GetInsertBlock();

    // The first argument is always a structural return pointer.
    vector<llvm::Type *> argTypes(irArgCount, types.RTAtomPtr);

    llvm::Function *func =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            llvm::Type::getVoidTy(ctx),
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    name,
                    &module);

    // Create the BB as soon as possible to capture allocas.
    auto *bb = llvm::BasicBlock::Create(ctx, "start", func);
    ir.SetInsertPoint(bb);

    for (long i = 0; i < irArgCount; ++i) {
        auto *irArg = func->arg_begin() + i;

        if (i == 0) {
            irArg->setName("rv");
            irArg->addAttr(llvm::Attribute::AttrKind::StructRet);
        } else {
            auto *argName = args[i - 1];
            irArg->setName(*argName);
            auto *argMemory = ir.CreateAlloca(types.RTAtom, 0, *argName);
            scope.slots[*argName] = argMemory;
            ir.CreateStore(irArg, argMemory);
        }
    }

    for (auto stmt : body) {
        switch (stmt->getType()) {
        case ASTType::StmtAssign: {
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            scope.slots[ident] = emitAlloca(RTNoneAtom(), ident);
            break;
        }

        default: break;
        }
    }

    llvm::Value *value;
    bool lastIsRet = false; // True if last statement was a return.

    for (auto stmt : body) {
        ir.SetInsertPoint(bb);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            // The result is a RTAtom.
            value = emit(ret.expr, module, scope);
            ir.CreateStore(value, func->arg_begin());
            ir.CreateRetVoid();
            lastIsRet = true;
        } else {
            value = emit(*stmt, module, scope);
            lastIsRet = false;
        }
    }

    if (!lastIsRet) {
        // Return None if not an explicit return.
        value = emitAlloca(RTNoneAtom());
        ir.CreateStore(value, func->arg_begin());
        ir.CreateRetVoid();
    }

    llvm::verifyFunction(*func);
    ir.SetInsertPoint(insertPoint);

    return new RTFuncObj(name, &scope, func);
}

llvm::AllocaInst *
Emitter::emitAlloca(RTAtom const &atom)
{
    return emitAlloca(atom, "");
}

/**
 * Emit a constant value on the stack.
 * @returns Pointer to the allocated region. **/
llvm::AllocaInst *
Emitter::emitAlloca(
        RTAtom const &atom,
        string const &name)
{
    auto *alloca = ir.CreateAlloca(types.RTAtom, 0, name);

    long tval = atom.getTypeValue();
    double dval;
    uint64_t ival;

    switch (atom.getType()) {
    case RTAtomType::None:
        ival = 0;
        break;

    case RTAtomType::Bool:
        ival = atom.getBoolValue() ? 1 : 0;
        break;

    case RTAtomType::Int:
        ival = (uint64_t) atom.getIntValue();
        break;

    case RTAtomType::Dec:
        dval = atom.getDecValue();
        ival = * ((uint64_t *) &dval);
        break;

    case RTAtomType::Obj:
        ival = (uint64_t) atom.getObjValue();
        break;

    default:
        throw "Oops!";
    }

    ir.CreateStore(
            llvm::ConstantStruct::get(
                    types.RTAtom,
                    {
                            llvm::ConstantInt::get(types.RTType, tval),
                            llvm::ConstantInt::get(types.RawPtr, ival),
                    }),
            alloca);

    return alloca;
}
