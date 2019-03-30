#pragma once

#include "PyObj.h"
#include <string>

#ifdef __cplusplus
namespace llvmPy {

class PyInt : public PyObj {
public:
    explicit PyInt(int64_t value);

    int64_t getValue() const;

    std::string py__str__() override;

    PyObj *py__add__(PyObj &rhs) override;

    PyObj *py__sub__(PyObj &rhs) override;

    PyObj *py__mul__(PyObj &rhs) override;

    bool py__eq__(PyObj &rhs) override;

    int64_t py__int__() override;

    bool py__bool__() override;

protected:
    int compare(PyObj &rhs) const override;

private:
    int64_t _value;
};

} // namespace llvmPy
#endif // __cplusplus
