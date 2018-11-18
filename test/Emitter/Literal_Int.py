# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

# CHECK-DAG: @PyInt.5 = private constant %PyObj*
# CHECK-DAG: @PyInt._6 = private constant %PyObj*

# CHECK: define
# CHECK-SAME: @__body__

print(5)
print(-6)

# CHECK: %PyInt.5 = load %PyObj*, %PyObj** @PyInt.5
# CHECK-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyInt.5)
# CHECK: %PyInt._6 = load %PyObj*, %PyObj** @PyInt._6
# CHECK-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyInt._6)
