# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# Check that the global isn't defined multiple times.
# The `print()` is necessary, as otherwise the value would be discarded.

# IR: @llvmPy_None = external constant %PyObj
# IR-NOT: @llvmPy_None
# IR-LABEL: define %PyObj* @__body__
print(None)
# IR: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_None)
print(None)
# IR-NEXT: {{%[0-9]+}} = call %PyObj* @llvmPy_print(%PyObj* @llvmPy_None)

# OUTPUT: None
# OUTPUT-NEXT: None
