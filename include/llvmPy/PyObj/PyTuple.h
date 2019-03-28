#pragma once

#include "PyObj.h"

#ifdef __cplusplus
namespace llvmPy {

class PyTuple : public PyObj {
public:
    static PyTuple &get(int64_t count, PyObj **members);

    PyTuple() noexcept;
    PyTuple(int64_t count, PyObj **members) noexcept;

    std::string py__str__() override;

    bool py__bool__() override;

    int64_t py__len__() override;

private:
    int64_t const _count;
    PyObj ** const _members;
};

} // namespace llvmPy

extern "C" {
#endif // __cplusplus

extern llvmPy::PyTuple llvmPy_tuple0;

#ifdef __cplusplus
}
#endif // __cplusplus
