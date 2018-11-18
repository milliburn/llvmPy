# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

print(1)

# CHECK-LABEL: define %PyObj* @__body__
# CHECK: call %PyObj* @llvmPy_print(%PyObj* %PyInt.1)
# CHECK-DAG: declare %PyObj* @llvmPy_print(%PyObj*)
