#pragma once

#include "PyObj.h"

#ifdef __cplusplus
namespace llvmPy {

class PyBool : public PyObj {
public:
    static PyBool &get(bool value);

    explicit PyBool(bool value) noexcept;

    bool getValue() const;

    std::string py__str__() override;

    bool py__bool__() override;

private:
    bool const _value;
};

} // namespace llvmPy

extern "C" {
#endif // __cplusplus

extern llvmPy::PyBool llvmPy_True;
extern llvmPy::PyBool llvmPy_False;

#ifdef __cplusplus
}
#endif // __cplusplus
