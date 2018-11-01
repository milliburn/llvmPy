# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as %t1 | llvm-dis | FileCheck %s

f = lambda: 1

# CHECK-DAG: %Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>
# CHECK-DAG: %Frame1 = type <{ %Frame1*, %FrameN*, [1 x %PyObj*] }>

# CHECK: define
# CHECK-SAME: @__body__
# CHECK-NEXT: %frame = alloca %Frame1

# CHECK: %4 = call %PyObj* @llvmPy_func(%Frame1* %frame, %PyObj* (%Frame1*)* @lambda)
# CHECK-NEXT: store %PyObj* %4, %PyObj** %3

# CHECK: define
# CHECK-SAME: @lambda (%Frame1* %outer)
# CHECK-NEXT: %frame = alloca %Frame0

# CHECK: [[rv:%[0-9]]] = calls %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: ret %PyObj* [[rv]]

# CHECK-DAG: declare %PyObj* @llvmPy_int(i64)
# CHECK-DAG: declare %PyObj* @llvmPy_func(%FrameN*, i64)
