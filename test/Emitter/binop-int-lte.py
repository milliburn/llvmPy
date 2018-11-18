# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

# CHECK: define
# CHECK-SAME: @__body__

1 <= 2

# CHECK: [[RV:%[0-9]]] = call %PyObj* @llvmPy_le(%PyObj* %PyInt.1, %PyObj* %PyInt.2)

# CHECK-DAG: declare %PyObj* @llvmPy_le(%PyObj*, %PyObj*)
