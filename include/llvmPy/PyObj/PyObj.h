#pragma once

#include <llvmPy/Typed.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

enum class PyObjType : long {
    None = 0,
    Int = 1,
    Str = 2,
    Bool = 3,
    Func = 4,
};

/**
 * The base class for all objects accessible on the heap at runtime.
 */
class PyObj : public Typed<PyObjType> {
public:
    PyObj(PyObjType type) : Typed(type) {}
    virtual std::string py__str__();

    virtual bool py__lt__(PyObj &rhs);
    virtual bool py__le__(PyObj &rhs);
    virtual bool py__eq__(PyObj &rhs);
    virtual bool py__ne__(PyObj &rhs);
    virtual bool py__ge__(PyObj &rhs);
    virtual bool py__gt__(PyObj &rhs);
};

} // namespace llvmPy
#endif // __cplusplus
