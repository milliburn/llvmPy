# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# Check that the global isn't defined multiple times.
# The `print()` is necessary, as otherwise the value would be discarded.

# IR: @llvmPy_True = external constant %PyObj
# IR-NOT: @llvmPy_True
# IR-LABEL: define %PyObj* @__body__
print(True)
# IR: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_True)
print(True)
# IR-NEXT: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_True)

# OUTPUT: True
# OUTPUT-NEXT: True

