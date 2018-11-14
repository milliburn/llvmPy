#pragma once

#include "PyObj.h"
#include <string>
#include <memory>

#ifdef __cplusplus
namespace llvmPy {

class PyStr : public PyObj {
public:
    static bool classof(Typed const *x) {
        return x->isType(PyObjType::Str);
    }

    explicit PyStr(std::unique_ptr<std::string const> value) noexcept;
    explicit PyStr(std::string const &value) noexcept;

    std::string const &getValue() const;

    std::string py__str__() override;

    bool py__bool__() override;

private:
    std::unique_ptr<std::string const> value;
};

} // namespace llvmPy
#endif // __cplusplus
