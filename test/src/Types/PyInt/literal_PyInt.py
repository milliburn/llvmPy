# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# IR-DAG: @PyInt.5 = private constant %PyObj*
# IR-DAG: @PyInt._6 = private constant %PyObj*

# IR: define
# IR-SAME: @__body__

print(5)
print(-6)

# IR: %PyInt.5 = load %PyObj*, %PyObj** @PyInt.5
# IR-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyInt.5)
# IR: %PyInt._6 = load %PyObj*, %PyObj** @PyInt._6
# IR-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyInt._6)

# OUTPUT: 5
# OUTPUT-NEXT: -6
