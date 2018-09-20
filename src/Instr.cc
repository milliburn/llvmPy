#include "llvm/ADT/APInt.h"
#include <llvm/IR/DataLayout.h>
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
            { RawPtr, RawPtr },
            "RTAtom",
            true);

    RTAtomPtr = llvm::PointerType::getUnqual(RTAtom);

    lpy_add = llvm::FunctionType::get(
            llvm::Type::getVoidTy(ctx),
            { RTAtomPtr, RTAtomPtr, RTAtomPtr },
            false);

}

extern "C" void
lpy_add(RTAtom * __restrict__ rv,
        RTAtom & __restrict__ lhs,
        RTAtom & __restrict__ rhs)
{
    switch (lhs.getType()) {
    case RTType::RTDecimalAtom:
    case RTType::RTDecimal:
        switch (rhs.getType()) {
        case RTType::RTDecimalAtom:
        case RTType::RTDecimal:
            *rv = RTAtom(lhs.atom.decimal + rhs.atom.decimal);
            return;

        default:
            return;
        }

    default:
        return;
    }

    throw "Oops!";
}

extern "C" void
lpy_eq(
        RTAtom * __restrict__ rv,
        RTAtom & __restrict__ lhs,
        RTAtom & __restrict__ rhs)
{
    switch (lhs.getType()) {

    case RTType::RTDecimalAtom:
    case RTType::RTDecimal:
        switch (rhs.getType()) {
        case RTType::RTDecimalAtom:
        case RTType::RTDecimal:
            *rv = RTAtom(lhs.atom.decimal == rhs.atom.decimal);
            return;
        default:
            *rv = RTAtom(false);
            return;
        }

    case RTType::RTBoolAtom:
    case RTType::RTBool:
        switch (rhs.getType()) {
        case RTType::RTBoolAtom:
        case RTType::RTBool:
            *rv = RTAtom(lhs.atom.boolean == rhs.atom.boolean);
            return;
        default:
            *rv = RTAtom(false);
            return;
        }

    default:
        *rv = RTAtom(false);
    }
}
