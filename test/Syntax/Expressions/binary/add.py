# RUN: test-ir.sh %s

1 + 2

# IR: define
# IR-SAME: @__body__

# IR: [[RV:%[0-9]]] = call %PyObj* @llvmPy_add(%PyObj* %PyInt.1, %PyObj* %PyInt.2)

# IR-DAG: declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
