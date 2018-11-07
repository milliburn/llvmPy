# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

f = lambda: 1

# CHECK-DAG: %Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>
# CHECK-DAG: %Frame1 = type <{ %Frame1*, %FrameN*, [1 x %PyObj*] }>

# CHECK: define
# CHECK-SAME: @__body__
# CHECK-NEXT: %outer = load %FrameN*, %FrameN** %outerptr
# CHECK-NEXT: %frame = alloca %Frame1

# CHECK: [[frame:%[0-9]]] = bitcast %Frame1* %frame to %FrameN*
# CHECK-NEXT: [[rv:%[0-9]]] = call %PyObj* @llvmPy_func(%FrameN* [[frame]], i8* bitcast (%PyObj* (%Frame1**)* @lambda to i8*))
# CHECK-NEXT: store %PyObj* [[rv]], %PyObj** %{{[0-9]}}

# CHECK: define
# CHECK-SAME: @lambda(%Frame1** %outerptr)
# CHECK-NEXT: %outer = load %Frame1*, %Frame1** %outerptr
# CHECK-NEXT: %frame = alloca %Frame0

# CHECK: [[rv:%[0-9]]] = call %PyObj* @llvmPy_int(i64 1)
# CHECK-NEXT: ret %PyObj* [[rv]]

# CHECK-DAG: declare %PyObj* @llvmPy_int(i64)
# CHECK-DAG: declare %PyObj* @llvmPy_func(%FrameN*, i8*)
