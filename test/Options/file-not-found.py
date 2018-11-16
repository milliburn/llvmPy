# RUN: not llvmPy %s.notexist 2>&1 | FileCheck %s

print("Test")

# CHECK-NOT: Test
# CHECK: Cannot open file
# CHECK-SAME: .notexist
# CHECK-SAME: file not found
# CHECK-NOT: Test
