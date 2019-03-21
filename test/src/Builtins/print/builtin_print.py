# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print(1)

# IR-LABEL: define %PyObj* @__body__
# IR: call %PyObj* @llvmPy_print(%PyObj* %PyInt.1)
# IR-DAG: declare %PyObj* @llvmPy_print(%PyObj*)

# OUTPUT: 1
