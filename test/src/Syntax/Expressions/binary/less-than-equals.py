# RUN: test-ir.sh %s

# IR: define
# IR-SAME: @__body__

1 <= 2

# IR: [[RV:%[0-9]]] = call %PyObj* @llvmPy_le(%PyObj* %PyInt.1, %PyObj* %PyInt.2)

# IR-DAG: declare %PyObj* @llvmPy_le(%PyObj*, %PyObj*)
