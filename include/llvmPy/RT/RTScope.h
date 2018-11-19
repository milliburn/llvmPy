#pragma once
#include <unordered_map>
#include <string>
#include <llvmPy/RT/Frame.h>
#include <llvmPy/Support/Testing.h>

#ifdef __cplusplus
namespace llvm {
class Function;
class LLVMContext;
class Module;
class Value;
class GlobalVariable;
} // namespace llvm

namespace llvmPy {

class Compiler;
class Types;
class RTModule;
class PyObj;

class RTScope {
public:
    struct Slot {
        llvm::Value *value;
        size_t frameIndex;
    };

    RTScope(RTModule &module,
            RTScope &parent,
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

    explicit RTScope(RTModule &module);

public:
    RTScope *createDerived(
            llvm::Value *innerFramePtr,
            llvm::Value *outerFramePtr);

public:
    RTModule &getModule() const;

    bool hasParent() const;

    __mock_virtual RTScope &getParent() const;

    llvm::Value *getOuterFramePtr() const;

    llvm::Value *getInnerFramePtr() const;

    bool hasSlot(std::string const &name) const;

    void declareSlot(std::string const &name);

    llvm::Value *getSlotValue(std::string const &name) const;

    void setSlotValue(std::string const &name, llvm::Value *value);

    size_t getSlotIndex(std::string const &name) const;

    size_t getSlotCount() const;

private:
    RTModule &module;
    RTScope * const parent;
    llvm::Value * const outerFramePtr;
    llvm::Value * const innerFramePtr;

    std::unordered_map<std::string, Slot> slots;

};

} // namespace llvmPy
#endif // __cplusplus

