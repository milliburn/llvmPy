# RUN: llvmPy %s > %t1
# RUN: cat -n %t1 >&2
# RUN: cat %t1 | FileCheck %s

print(1 <= 2)
# CHECK: True
print(1 < 2)
# CHECK-NEXT: True
print(1 == 2)
# CHECK-NEXT: False
print(1 != 2)
# CHECK-NEXT: True
print(1 >= 2)
# CHECK-NEXT: False
print(1 > 2)
# CHECK-NEXT: False
print(2 > 1)
# CHECK-NEXT: True
print(2 >= 1)
# CHECK-NEXT: True
print(2 == 1)
# CHECK-NEXT: False
print(2 != 1)
# CHECK-NEXT: True
print(2 <= 1)
# CHECK-NEXT: False
print(2 < 1)
# CHECK-NEXT: False
print(2 <= 2)
# CHECK-NEXT: True
print(2 >= 2)
# CHECK-NEXT: True
print(2 == 2)
# CHECK-NEXT: True
print(2 != 2)
# CHECK-NEXT: True
