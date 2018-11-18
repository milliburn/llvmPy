# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s


# Make sure the constant wasn't linked twice.

# CHECK: @PyInt.1 = private constant %PyObj*
# CHECK-NOT: @PyInt.1

x = 1
f = lambda: x + 1

# CHECK-LABEL: define %PyObj* @__body__
# CHECK-NEXT: %outer = load %FrameN*, %FrameN** %outerptr
# CHECK-NEXT: alloca %Frame2
# CHECK: %PyInt.1 = load %PyObj*, %PyObj** @PyInt.1
# CHECK-NEXT: store %PyObj* %PyInt.1, %PyObj** %var_x
# CHECK: [[RV:%[0-9]+]] = call %PyObj* @llvmPy_func
# CHECK-NEXT: store %PyObj* [[RV]], %PyObj** %{{[a-z_0-9]+}}

# CHECK-LABEL: define %PyObj* @lambda
# CHECK-NEXT: %outer = load %Frame2*, %Frame2** %outerptr
# CHECK-NEXT: alloca %Frame0
# CHECK: [[OUTER:%[0-9]+]] = load %Frame2*, %Frame2** %outer
# CHECK-NEXT: [[GEP:%[0-9]+]] = getelementptr %Frame2, %Frame2* [[OUTER]], i64 0, i32 2, i64 1
# CHECK-NEXT: [[X:%[0-9]+]] = load %PyObj*, %PyObj** [[GEP]]
# CHECK: [[RV:%[0-9]+]] = call %PyObj* @llvmPy_add(%PyObj* [[X]], %PyObj* %PyInt.1)
# CHECK-NEXT: ret %PyObj* [[RV]]
