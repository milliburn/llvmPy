# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

print(1)

# CHECK-LABEL: define %PyObj* @__body__
# CHECK: [[_1:%[0-9]+]] = call %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: [[_2:%[0-9]+]] = call %PyObj* @llvmPy_print(%PyObj* [[_1]])

# CHECK-DAG: declare %PyObj* @llvmPy_print(%PyObj*)
