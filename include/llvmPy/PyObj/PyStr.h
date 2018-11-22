#pragma once

#include "PyObj.h"
#include <string>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class PyStr : public PyObj {
public:
    explicit PyStr(std::unique_ptr<std::string const> value) noexcept;

    explicit PyStr(std::string const &value) noexcept;

    std::string const &getValue() const;

    std::string const &str() const;

    std::string py__str__() override;

    int64_t py__len__() override;

    int64_t py__int__() override;

    bool py__bool__() override;

    bool py__eq__(PyObj &rhs) override;

    PyObj *py__add__(PyObj &rhs) override;

    PyObj *py__getattr__(std::string const &name) override;

    static PyObj *py_upper(PyStr &self);

    static PyObj *py_capitalize(PyStr &self);

private:
    std::unique_ptr<std::string const> _value;
};

} // namespace llvmPy
#endif // __cplusplus
