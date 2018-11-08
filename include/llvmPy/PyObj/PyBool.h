#pragma once

#include "PyObj.h"

#ifdef __cplusplus
namespace llvmPy {

class PyBool : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Bool);
    }

    explicit PyBool(bool value) noexcept;
    static PyBool &get(bool value);
    bool getValue() const;
    virtual std::string py__str__() override;

private:
    bool const value;
};

} // namespace llvmPy
#endif // __cplusplus
