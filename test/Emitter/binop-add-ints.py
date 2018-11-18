# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

1 + 2

# CHECK: define
# CHECK-SAME: @__body__

# CHECK-NEXT: [[RV:%[0-9]]] = call %PyObj* @llvmPy_add(%PyObj* @PyInt.1, %PyObj* @PyInt.2)

# CHECK-DAG: declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
