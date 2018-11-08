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
    virtual std::string py__str__() override;

private:
    int64_t value;
};

} // namespace llvmPy
#endif // __cplusplus
