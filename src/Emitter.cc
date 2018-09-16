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

Emitter::Emitter(RT &rt, llvm::DataLayout &dl) noexcept
: ir(ctx), rt(rt), dataLayout(dl), types(ctx, dl)
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
        auto *value = scope.slots[ident];

        if (!value) {
            throw SyntaxError(
                    "Slot " + ident + " for assignment does not exist");
        }

        if (value->ir) {
            throw SyntaxError(
                    "Slot " + ident + " has already been assigned");
        }

        value->ir = emit(stmt.rhs, module, scope);
        return value->ir;
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

        if (!value->ir) {
            throw SyntaxError(
                    "IR for identifier " + ident.name +
                    " has not been emitted");
        }

        return value->ir;
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
        auto *func = emitFunction(ast, module, scope);
        // Assign module to a slot.
        return func;
    }

    case ASTType::ExprLambda:
        return emitFunction(ast, module, scope);

    case ASTType::ExprBinary: {
        auto &expr = cast<BinaryExpr>(ast);
        auto *lhs = emit(expr.lhs, module, scope);
        auto *rhs = emit(expr.rhs, module, scope);

        switch (expr.op) {
        case tok_add:
            return ir.CreateCall(
                    module.getOrInsertFunction("lpy_add", types.lpy_add),
                    { lhs, rhs });

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
    RTScope &scope = rt.createScope();

    // Scan the module for assignment to module-level slots,
    // which is used to pre-populate global scope.
    //
    // Then emit a special slot, __body__, to which the body
    // statements are bound. __body__ is executed as soon as
    // the module has been loaded.

    for (auto stmt : stmts) {
        switch (stmt->getType()) {

        case ASTType::StmtAssign: {
            // Pre-register slots referenced in assignments.
            auto &assign = *cast<AssignStmt>(stmt);
            auto &ident = assign.lhs;
            scope.slots.insert(make_pair(ident, new RTValue()));
            break;
        }

        default: break;
        }
    }

    // Emit bytecode for the module prototype and body.
    vector<llvm::Type *> types;

    llvm::FunctionType *fty =
            llvm::FunctionType::get(
                    llvm::Type::getDoubleTy(ctx),
                    types,
                    false);

    llvm::Function *f =
            llvm::Function::Create(
                    fty,
                    llvm::Function::ExternalLinkage,
                    "__body__",
                    &module);

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "start", f);
    llvm::Value *value = nullptr;

    for (auto stmt : stmts) {
        ir.SetInsertPoint(bb);
        value = emit(*stmt, module, scope);
    }

    ir.CreateRet(value);
    llvm::verifyFunction(*f);

    return &module;
}

RTFunc *
Emitter::emit(
        AST const &ast,
        llvm::Module &module,
        RTScope &scope)
{
    RTScope &inner = rt.createScope(scope);
    vector<Stmt *> body;
    vector<string const *> args;
    string name;

    if (isa<LambdaExpr>(ast)) {
        auto &lambda = cast<LambdaExpr>(ast);
        args = lambda.args;
        body.push_back(new ReturnStmt(lambda.expr));
        name = "lambda";
    } else if (isa<DefStmt>(ast)) {
        auto &def = cast<DefStmt>(ast);
        name = def.name;
        body = def.stmts;
        args = def.args;
    } else {
        throw EmitterError("Unknown function AST");
    }

    for (auto arg : args) {
        inner.addSlot(*arg);
    }

    vector<llvm::Type *> argTypes(args.size(), types.RTAtom);

    llvm::Function *function =
            llvm::Function::Create(
                    llvm::FunctionType::get(
                            types.RTAtom,
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
        auto &name = *args[iArg];
        arg.setName(name);
        llvm::AllocaInst *alloca = ir.CreateAlloca(types.RTAtom);
        ir.CreateStore(&arg, alloca);
        // TODO: Finish initialising the RTAtom on the stack.
        inner.slots[name]->ir = alloca;
    }

    for (auto stmt : body) {
        ir.SetInsertPoint(bb);

        if (isa<ReturnStmt>(stmt)) {
            auto &ret = *cast<ReturnStmt>(stmt);
            value = emit(ret.expr, module, inner);
            ir.CreateRet(value);
        } else {
            emit(*stmt, module, inner);
        }
    }

    llvm::verifyFunction(*function);

    return new RTFunc(name, function);
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
