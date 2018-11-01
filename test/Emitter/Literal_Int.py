# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

1

# CHECK: define
# CHECK-SAME: @__body__

# CHECK: %{{[0-9]}} = call %PyObj* @llvmPy_int(i64 1)

# CHECK-DAG: declare %PyObj* @llvmPy_int(i64)
