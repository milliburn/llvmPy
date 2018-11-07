# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

def func(x):
    y = x + 2
    return y + 3

print(func(1))
# CHECK: 6
