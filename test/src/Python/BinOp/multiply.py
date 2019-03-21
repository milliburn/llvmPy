# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

print(1 * 2)
# CHECK: 2
print(2 * 2)
# CHECK-NEXT: 4
print(0 * 2)
# CHECK-NEXT: 0
