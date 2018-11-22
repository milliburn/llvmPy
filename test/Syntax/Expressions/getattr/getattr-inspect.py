# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print("test".upper)

# IR-LABEL: @__body__
# IR: [[str:%[^ ]+]] = load %PyObj*, %PyObj** @PyStr.0
# IR: [[upper:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[str]], %PyObj* %PyStr.1)
# IR: @llvmPy_print(%PyObj* [[upper]])

# OUTPUT: <PyFunc 0x{{[0-9A-F]+}}>
