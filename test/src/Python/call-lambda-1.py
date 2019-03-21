# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

f = lambda x: x + 1
print(f(1))
# CHECK: 2
