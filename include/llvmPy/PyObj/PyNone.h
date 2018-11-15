#pragma once

#include "PyObj.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyNone : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::None);
    }

    static PyNone &get();

    PyNone() noexcept;

    std::string py__str__() override;

    bool py__bool__() override;
};

} // namespace llvmPy
#endif // __cplusplus
