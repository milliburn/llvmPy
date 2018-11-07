# RUN: llvmPy --ir %s | llc -march=x86 > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

# Notice triple underscore at the beginning.
# CHECK: .globl ___body__
# CHECK-LABEL: ___body__:
