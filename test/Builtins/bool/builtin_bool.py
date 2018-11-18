# RUN: test-ir.sh %s

print(bool(1))
# IR: [[RV:%[0-9]+]] = call %PyObj* @llvmPy_bool(%PyObj* %PyInt.1)
# IR: call %PyObj* @llvmPy_print(%PyObj* [[RV]])

# CHECK-DAG: declare %PyObj* @llvmPy_bool(%PyObj*)
