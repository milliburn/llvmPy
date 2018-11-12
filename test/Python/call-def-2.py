# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

def f(x, y):
    return x + y + 1

print(f(2, 3))
# CHECK: 6
