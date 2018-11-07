# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

f = lambda: 1
print(f())
# CHECK: 1
