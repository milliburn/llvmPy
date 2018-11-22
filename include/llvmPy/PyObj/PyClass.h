#pragma once

#include "PyObj.h"

#ifdef __cplusplus
namespace llvmPy {

class PyClass : public PyObj {
public:
    bool isInstance() const override;
};

} // namespace llvmPy
#endif // __cplusplus
