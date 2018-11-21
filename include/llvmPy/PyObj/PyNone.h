#pragma once

#include "PyObj.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyNone : public PyObj {
public:
    static PyNone &get();

    PyNone() noexcept;

    std::string py__str__() override;

    bool py__bool__() override;
};

} // namespace llvmPy

extern "C" {
#endif // __cplusplus

extern llvmPy::PyNone llvmPy_None;

#ifdef __cplusplus
}
#endif // __cplusplus
