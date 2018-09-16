#include "llvm/ADT/APInt.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvmPy/Instr.h>
#include <llvmPy/RT.h>
using namespace llvmPy;
using llvm::cast;

Types::Types(
        llvm::LLVMContext &ctx,
        llvm::DataLayout const &dl)
{
    RawPtr = llvm::IntegerType::get(ctx, dl.getPointerSize());

    RTAtom = llvm::StructType::create(
            ctx,
            { (llvm::Type *) &RawPtr, (llvm::Type *) &RawPtr },
            "RTAtom",
            true);

    RTAtomPtr = llvm::PointerType::getUnqual(RTAtom);

    lpy_add = llvm::FunctionType::get(
            RTAtom,
            { RTAtomPtr, RTAtomPtr },
            false);
}

extern "C" llvm::Function *
lpy_getfuncptr(RTAtom &atom)
{
    switch (atom.getType()) {
    case RTType::RTPtrAtom:
        RTAny &inner = *atom.atom.any;
        switch (inner.getType()) {
        case RTType::RTFunc:
            return cast<RTFunc>(inner).ir;
        }
    }
}

extern "C" RTAtom
lpy_add(RTAtom &lhs, RTAtom &rhs)
{
    switch (lhs.getType()) {
    case RTType::RTDecimalAtom:
    case RTType::RTDecimal:
        switch (rhs.getType()) {
        case RTType::RTDecimalAtom:
        case RTType::RTDecimal:
            return RTAtom(lhs.atom.decimal + rhs.atom.decimal);
        }
    }
}

extern "C" RTAtom
lpy_eq(RTAtom &lhs, RTAtom &rhs)
{
    switch (lhs.getType()) {

    case RTType::RTDecimalAtom:
    case RTType::RTDecimal:
        switch (rhs.getType()) {
        case RTType::RTDecimalAtom:
        case RTType::RTDecimal:
            return RTAtom(lhs.atom.decimal == rhs.atom.decimal);
        default:
            return RTAtom(false);
        }

    case RTType::RTBoolAtom:
    case RTType::RTBool:
        switch (rhs.getType()) {
        case RTType::RTBoolAtom:
        case RTType::RTBool:
            return RTAtom(lhs.atom.boolean == rhs.atom.boolean);
        default:
            return RTAtom(false);
        }

    default:
        return RTAtom(false);
    }
}
