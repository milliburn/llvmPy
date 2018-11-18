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

    std::string py__str__() override;

    int64_t py__len__() override;

    int64_t py__int__() override;

    bool py__bool__() override;

    bool py__eq__(PyObj &rhs) override;

    PyObj *py__add__(PyObj &rhs) override;

private:
    std::unique_ptr<std::string const> value;
};

} // namespace llvmPy
#endif // __cplusplus
