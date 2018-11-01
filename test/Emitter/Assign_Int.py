# RUN: llvmPy --ir %s | llvm-as | llvm-dis | FileCheck %s

x = 1

# CHECK: define
# CHECK-SAME: @__body__

# CHECK-NEXT: %frame = alloca %Frame1
# CHECK: %[[op:[0-9]+]] = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 2, i64 0
# CHECK: store %PyObj* null, %PyObj** %3

# CHECK: %[[ret:[0-9]+]] = call %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: store %PyObj* %[[ret]], %PyObj** %[[op]]

# CHECK: ret %PyObj* null

# CHECK-DAG: declare %PyObj* @llvmPy_int(i64)
