# RUN: llvmPy -c 'print("Test!")' | FileCheck %s
# RUN: llvmPy %s | FileCheck %s
print("Test!")
# CHECK: Test!
