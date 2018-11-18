# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

# CHECK: @PyStr.0 = private constant %PyObj*

# CHECK-LABEL: define %PyObj* @__body__

print("Greetings, programs!")

# CHECK: %PyStr.0 = load %PyObj*, %PyObj** @PyStr.0
# CHECK-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyStr.0)


