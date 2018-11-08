# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

1 <= 2

# CHECK: define
# CHECK-SAME: @__body__

# CHECK: [[LHS:%[0-9]]] = call %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: [[RHS:%[0-9]]] = call %PyObj* @llvmPy_int(i64 2)
# CHECK-NEXT: [[RV:%[0-9]]] = call %PyObj* @llvmPy_le(%PyObj* [[LHS]], %PyObj* [[RHS]])

# CHECK-DAG: declare %PyObj* @llvmPy_le(%PyObj*, %PyObj*)
