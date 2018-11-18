# RUN: test-ir.sh %s

print(str(1))
# IR: [[RV:%[0-9]+]] = call %PyObj* @llvmPy_str(%PyObj* %PyInt.1)
# IR: call %PyObj* @llvmPy_print(%PyObj* [[RV]])

# CHECK-DAG: declare %PyObj* @llvmPy_str(%PyObj*)
