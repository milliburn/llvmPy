#pragma once

#include <llvmPy/Typed.h>
#include <string>

#ifdef __cplusplus
namespace llvmPy {

/**
 * The base class for all objects accessible on the heap at runtime.
 */
class PyObj : public Typed {
public:
    PyObj();

    virtual std::string py__str__();

    virtual bool py__bool__();

    virtual int64_t py__int__();

    virtual int64_t py__len__();

    virtual PyObj * py__add__(PyObj &rhs);
    virtual PyObj * py__sub__(PyObj &rhs);
    virtual PyObj * py__mul__(PyObj &rhs);

    virtual bool py__lt__(PyObj &rhs);
    virtual bool py__le__(PyObj &rhs);
    virtual bool py__eq__(PyObj &rhs);
    virtual bool py__ne__(PyObj &rhs);
    virtual bool py__ge__(PyObj &rhs);
    virtual bool py__gt__(PyObj &rhs);
};

} // namespace llvmPy
#endif // __cplusplus
