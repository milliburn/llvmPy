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
    bool const value;
};

} // namespace llvmPy
#endif // __cplusplus
