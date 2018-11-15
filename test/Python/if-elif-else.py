# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

x = 2

if x == 1:
    print("A")
elif x == 2:
    print("B")
else:
    print("C")

# CHECK: B
