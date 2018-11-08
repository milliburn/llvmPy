# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

True

# CHECK: define
# CHECK-SAME: @__body__

# CHECK: %{{[0-9]}} = call %PyObj* @llvmPy_bool(i64 1)

# CHECK-DAG: declare %PyObj* @llvmPy_bool(i64)
