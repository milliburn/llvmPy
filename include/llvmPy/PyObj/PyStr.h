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

    explicit PyStr(std::unique_ptr<std::string const> value)
    : PyObj(PyObjType::Str), value(std::move(value))
    {}

    std::string const &getValue() const { return *value; }
    virtual std::string py__str__() override;

private:
    std::unique_ptr<std::string const> value;
};

} // namespace llvmPy
#endif // __cplusplus
