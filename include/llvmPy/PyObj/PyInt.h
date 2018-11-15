#pragma once

#include "PyObj.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyInt : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Int);
    }

    explicit PyInt(int64_t value);
    int64_t getValue() const;
    std::string py__str__() override;
    PyObj &py__add__(PyObj &rhs) override;
    PyObj &py__mul__(PyObj &rhs) override;
    bool py__lt__(PyObj &rhs) override;
    bool py__le__(PyObj &rhs) override;
    bool py__eq__(PyObj &rhs) override;
    bool py__ge__(PyObj &rhs) override;
    bool py__gt__(PyObj &rhs) override;

    bool py__bool__() override;

private:
    static constexpr int InvalidCmp = ~0;

    int64_t value;
    int cmp(PyObj &rhs, bool throwIfInvalid);
};

} // namespace llvmPy
#endif // __cplusplus
