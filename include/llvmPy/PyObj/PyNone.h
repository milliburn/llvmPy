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

    PyNone() noexcept;
    std::string py__str__() override;

    static PyNone &get();
};

} // namespace llvmPy
#endif // __cplusplus
