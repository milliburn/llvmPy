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
    RawPtr = llvm::IntegerType::get(ctx, dl.getPointerSizeInBits());

    RTType = RawPtr;

    RTAtom = llvm::StructType::create(
            ctx,
            { RTType, RawPtr },
            "RTAtom",
            true);

    RTAtomPtr = llvm::PointerType::getUnqual(RTAtom);

    lpy_add = llvm::FunctionType::get(
            llvm::Type::getVoidTy(ctx),
            { RTAtomPtr, RTAtomPtr, RTAtomPtr },
            false);

}

extern "C" void
lpy_call(
        RTAtom * __restrict__ rv,
        RTAtom & __restrict__ callee)
{
    auto &func = cast<RTFuncObj>(*cast<RTObjAtom>(callee).getObjValue());
}

extern "C" void
lpy_add(RTAtom * __restrict__ rv,
        RTAtom & __restrict__ lhs,
        RTAtom & __restrict__ rhs)
{
    switch (lhs.getType()) {

    case RTAtomType::Int:
        switch (rhs.getType()) {
        case RTAtomType::Int:
            *rv = RTIntAtom(lhs.getIntValue() + rhs.getIntValue());
            return;

        case RTAtomType::Dec:
            *rv = RTDecAtom((double) lhs.getIntValue() + rhs.getDecValue());
            return;
        }

    case RTAtomType::Dec:
        switch (rhs.getType()) {
        case RTAtomType::Int:
            *rv = RTDecAtom(lhs.getDecValue() + (double) rhs.getDecValue());
            return;

        case RTAtomType::Dec:
            *rv = RTDecAtom(lhs.getDecValue() + rhs.getDecValue());
            return;

        default:
            return;
        }

    default:
        return;
    }
}

extern "C" void
lpy_eq(
        RTAtom * __restrict__ rv,
        RTAtom & __restrict__ lhs,
        RTAtom & __restrict__ rhs)
{
    switch (lhs.getType()) {

    case RTAtomType::Dec:
        switch (rhs.getType()) {
        case RTAtomType::Dec:
            *rv = RTBoolAtom(lhs.getDecValue() == rhs.getDecValue());
            return;
        default:
            *rv = RTBoolAtom(false);
            return;
        }

    case RTAtomType::Bool:
        switch (rhs.getType()) {
        case RTAtomType::Bool:
            *rv = RTBoolAtom(lhs.getBoolValue() == rhs.getBoolValue());
            return;

        default:
            *rv = RTBoolAtom(false);
            return;
        }

    default:
        *rv = RTBoolAtom(false);
    }
}
