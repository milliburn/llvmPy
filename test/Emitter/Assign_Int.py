# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

# CHECK-DAG: @PyInt.1 = private constant %PyObj

# CHECK: define
# CHECK-SAME: @__body__
# CHECK-NEXT: %outer = load %FrameN*, %FrameN** %outerptr
# CHECK-NEXT: %frame = alloca %Frame1
# CHECK: [[VAR_X:%[a-z_0-9]+]] = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 2, i64 0
# CHECK: store %PyObj* null, %PyObj** [[VAR_X]]

x = 1

# CHECK: %PyInt.1 = load %PyObj*, %PyObj** @PyInt.1
# CHECK-NEXT: store %PyObj* %PyInt.1, %PyObj** [[VAR_X]]
# CHECK: ret %PyObj* @llvmPy_None
