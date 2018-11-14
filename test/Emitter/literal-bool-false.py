# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

# Check that the global isn't defined multiple times.
# The `print()` is necessary, as otherwise the value would be discarded.

# CHECK: @llvmPy_False = external constant %PyObj
# CHECK-NOT: @llvmPy_False
# CHECK-LABEL: define %PyObj* @__body__
print(False)
# CHECK: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_False)
print(False)
# CHECK-NEXT: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_False)
