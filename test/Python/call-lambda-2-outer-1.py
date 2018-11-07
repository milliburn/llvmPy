# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

z = 1
f = lambda x, y: x + y + z
print(f(2, 3))
# CHECK: 6
