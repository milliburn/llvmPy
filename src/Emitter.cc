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
Emitter::emit(
        AST const &ast,
        llvm::Module &module,
        RTScope &scope)
{
    switch (ast.getType()) {

    /* Constant expressions */

    case ASTType::ExprDecLit: {
        auto &expr = cast<DecLitExpr>(ast);
        return alloca(expr.value);
    }

    case ASTType::ExprIntLit: {
        // TODO: Proper integers.
        auto &expr = cast<IntLitExpr>(ast);
        return alloca((double) expr.value);
    }

    case ASTType::ExprBoolLit: {
        auto &expr = cast<BoolLitExpr>(ast);
        return alloca(expr.value);
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

        return ir.CreateLoad(value, ident.name);
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

RTModule *
Emitter::emitModule(
        vector<Stmt *> const &stmts,
        string const &name)
{
    llvm::Module &ir = *new llvm::Module(name, ctx);

    RTFunc &func = *emitFunction(
            "__body__",
            {},
            stmts,
            ir,
            nullptr);

    RTModule *rtmodule = new RTModule(
            name,
            func.scope,
            func,
            ir);

    rt.modules.insert(make_pair(name, rtmodule));

    return rtmodule;
}

RTFunc *
Emitter::emitFunction(
        string const &name,
        vector<string const *> const args,
        vector<Stmt *> const body,
        llvm::Module &module,
        RTScope * const outer)
{
    RTScope &scope = *new RTScope(outer);

    // The first argument is always a structural return pointer.
    vector<llvm::Type *> argTypes(1 + args.size(), types.RTAtomPtr);

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

    for (auto *arg : args) {
        ir.SetInsertPoint(bb);
        scope.slots[*arg] = alloca(RTNone::getInstance());
    }

    for (auto stmt : body) {
        switch (stmt->getType()) {
        case ASTType::StmtAssign: {
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            ir.SetInsertPoint(bb);
            scope.slots[ident] = alloca(RTNone::getInstance());
            break;
        }

        default: break;
        }
    }

    llvm::Value *value;

    for (auto *arg = func->arg_begin(); arg < func->arg_end(); arg++) {
        arg->setName(name);

        if (arg == func->arg_begin()) {
            arg->addAttr(llvm::Attribute::AttrKind::StructRet);
        } else {
            // TODO: Store the argument in the pre-allocated slot above.
            // ir.CreateStore(arg, alloca);
        }
    }

    for (auto stmt : body) {
        ir.SetInsertPoint(bb);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            // The result is a RTAtom.
            value = emit(ret.expr, module, scope);
            ir.SetInsertPoint(bb);
            ir.CreateStore(value, func->arg_begin());
            ir.CreateRetVoid();
        } else {
            emit(*stmt, module, scope);
        }
    }

    // Catch-all at the end.
    ir.SetInsertPoint(bb);
    ir.CreateRetVoid();

    llvm::verifyFunction(*func);

    return new RTFunc(name, scope, func);
}

llvm::Value *
Emitter::address(llvmPy::RTAny &any)
{
    return address(&any);
}

llvm::Value *
Emitter::address(llvmPy::RTAny *any)
{
    return llvm::ConstantInt::get(types.RawPtr, (uint64_t) any);
}

/**
 * Emit a constant value on the stack.
 * @returns Pointer to the allocated region. **/
llvm::AllocaInst *
Emitter::alloca(RTAtom const &atom)
{
    auto *alloca = ir.CreateAlloca(types.RTAtom);

    uint16_t tval = (uint16_t) atom.getTypeValue();
    uint64_t ival;

    switch (atom.getType()) {
    case RTType::RTBoolAtom:
        ival = atom.atom.boolean ? 1 : 0;
        break;

    case RTType::RTIntegerAtom:
        ival = (uint64_t) atom.atom.integer;
        break;

    case RTType::RTDecimal:
    case RTType::RTDecimalAtom:
        ival = * ((uint64_t *) &atom.atom.decimal);
        break;

    case RTType::RTNone:
    case RTType::RTNoneAtom:
        ival = 0;
        break;

    case RTType::RTPtrAtom:
        ival = (uint64_t) atom.atom.any;
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

llvm::Value *
Emitter::ptr(RTAtom const &atom)
{
    return llvm::ConstantInt::get(
            types.RawPtr,
            reinterpret_cast<uint64_t >(&atom));
}
