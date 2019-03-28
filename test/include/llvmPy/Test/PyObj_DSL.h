#pragma once
#include <llvmPy/PyObj/PyObj.h>

static inline bool
operator< (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__lt__(r);
}

static inline bool
operator<= (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__le__(r);
}

static inline bool
operator== (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__eq__(r);
}

static inline bool
operator!= (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__ne__(r);
}

static inline bool
operator>= (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__ge__(r);
}

static inline bool
operator> (llvmPy::PyObj const &lhs, llvmPy::PyObj const &rhs)
{
    auto &l = const_cast<llvmPy::PyObj &>(lhs);
    auto &r = const_cast<llvmPy::PyObj &>(rhs);
    return l.py__gt__(r);
}
