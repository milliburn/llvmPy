# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s --match-full-lines

# CHECK-DAG: %PyObj = type opaque
# CHECK-DAG: %FrameN = type opaque
# CHECK-DAG: %Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>

# CHECK: define %PyObj* @__body__(%FrameN* %outer) prefix i64 {{[0-9]+}} {
# CHECK-NEXT: %frame = alloca %Frame0
# CHECK-NEXT: %1 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 0
# CHECK-NEXT: store %Frame0* %frame, %Frame0** %1
# CHECK-NEXT: %2 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 1
# CHECK-NEXT: store %FrameN* %outer, %FrameN** %2
# CHECK-NEXT: ret %PyObj* null
# CHECK-NEXT: }
