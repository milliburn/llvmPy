#pragma once

#include <llvmPy/PyObj/PyObj.h>

#ifdef __cplusplus
namespace llvmPy {

class PyTuple : public PyObj {
public:
    static PyTuple &get(int64_t count, PyObj **members);

    PyTuple() noexcept;
    PyTuple(int64_t count, PyObj **members) noexcept;

    bool isEmpty() const;
    int64_t getLength() const;
    PyObj &at(int64_t index) const;

public:
    std::string py__str__() override;

    bool py__bool__() override;

    int64_t py__len__() override;

    bool py__eq__(PyObj &rhs) override;

    bool py__lt__(PyObj &rhs) override;

    bool py__gt__(PyObj &rhs) override;

private:
    int64_t const _count;
    PyObj ** const _members;

    bool compareTo(PyTuple &rhs, bool (PyObj::* comparator)(PyObj &)) const;
};

} // namespace llvmPy

extern "C" {
#endif // __cplusplus

extern llvmPy::PyTuple llvmPy_tuple0;

#ifdef __cplusplus
}
#endif // __cplusplus
