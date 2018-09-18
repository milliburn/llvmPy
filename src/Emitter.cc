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

    case ASTType::StmtAssign: {
        auto &stmt = cast<AssignStmt>(ast);
        auto &ident = stmt.lhs;
        auto *slot = scope.slots[ident];
        auto *rhs = emit(stmt.rhs, module, scope);
        return ir.CreateStore(rhs, slot);
    }

    case ASTType::ExprDecLit: {
        auto &expr = cast<DecLitExpr>(ast);
        return emit(RTAtom(expr.value));
    }

    case ASTType::ExprIntLit: {
        // TODO: Proper integers.
        auto &expr = cast<IntLitExpr>(ast);
        return emit(RTAtom((double) expr.value));
    }

    case ASTType::ExprBoolLit: {
        auto &expr = cast<BoolLitExpr>(ast);
        return emit(RTAtom(expr.value));
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
                scope);
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
                scope);
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

llvm::Module *
Emitter::emitModule(
        vector<Stmt *> const &stmts,
        string const &name)
{
    llvm::Module &module = *new llvm::Module(name, ctx);
    RTFunc &func = *emitFunction(
            "__body__",
            {},
            stmts,
            module,
            RTScope::getNullScope());

    return &module;
}

RTFunc *
Emitter::emitFunction(
        string const &name,
        vector<string const *> args,
        vector<Stmt *> body,
        llvm::Module &module,
        RTScope &outer)
{
    RTScope &scope = *new RTScope(outer);

    for (auto stmt : body) {
        switch (stmt->getType()) {
        case ASTType::StmtAssign: {
            // Pre-register all assigned identifiers in the scope.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            scope.slots[ident] = nullptr;
            break;
        }

        default: break;
        }
    }

    vector<llvm::Type *> argTypes(args.size() + 1, types.RTAtomPtr);

    llvm::Function *function =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            llvm::Type::getVoidTy(ctx),
                            argTypes,
                            false),
                    llvm::Function::ExternalLinkage,
                    name,
                    &module);

    auto *bb = llvm::BasicBlock::Create(ctx, "start", function);
    ir.SetInsertPoint(bb);
    llvm::Value *value;

    int iArg = 0;
    for (auto &arg : function->args()) {
        if (iArg == 0) {
            arg.addAttr(llvm::Attribute::AttrKind::StructRet);
        }

        auto &name = *args[iArg++];
        arg.setName(name);
        llvm::AllocaInst *alloca = ir.CreateAlloca(types.RTAtom, 0, name);
        ir.CreateStore(&arg, alloca);
        scope.slots[name] = alloca;
    }

    for (auto stmt : body) {
        ir.SetInsertPoint(bb);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            // The result is a RTAtom.
            value = emit(ret.expr, module, scope);


            ir.CreateStore(value, function->arg_begin());
            ir.CreateRetVoid();
        } else {
            emit(*stmt, module, scope);
        }
    }

    llvm::verifyFunction(*function);

    return new RTFunc(name, scope, function);
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

llvm::Value *
Emitter::emit(RTAtom const &atom)
{
    return llvm::ConstantStruct::get(
            types.RTAtom,
            llvm::ConstantInt::get(
                    types.RawPtr,
                    static_cast<uint64_t>(atom.getType())),
            llvm::ConstantInt::get(
                    types.RawPtr,
                    reinterpret_cast<uint64_t>(atom.atom.any)));
}

llvm::Value *
Emitter::ptr(RTAtom const &atom)
{
    return llvm::ConstantInt::get(
            types.RawPtr,
            reinterpret_cast<uint64_t >(&atom));
}
