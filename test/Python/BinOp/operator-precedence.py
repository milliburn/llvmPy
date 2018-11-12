# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

print(2 + 3 * 4)
# CHECK: 14
print(2 * 3 + 4)
# CHECK-NEXT: 10
