# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

x = 1

# CHECK: define
# CHECK-SAME: @__body__
# CHECK-NEXT: %outer = load %FrameN*, %FrameN** %outerptr
# CHECK-NEXT: %frame = alloca %Frame1
# CHECK: %[[op:[a-z_0-9]+]] = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 2, i64 0
# CHECK: store %PyObj* null, %PyObj** %[[op]]

# CHECK: %[[ret:[0-9]+]] = call %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: store %PyObj* %[[ret]], %PyObj** %[[op]]

# CHECK: ret %PyObj* @llvmPy_None

# CHECK-DAG: declare %PyObj* @llvmPy_int(i64)
