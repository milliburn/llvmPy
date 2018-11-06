# RUN: llvmPy --ret %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

1
# CHECK: 1
