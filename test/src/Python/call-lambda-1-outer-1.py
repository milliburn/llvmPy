# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

y = 1
f = lambda x: x + y + 1
print(f(1))
# CHECK: 3
