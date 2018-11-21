# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print("test".upper)

# IR-LABEL: @__body__
# IR: [[str:%[^ ]+]] = load %PyObj*, %PyObj** @PyStr.0
# IR: [[upper:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[str]], i8* getelementptr inbounds ([6 x i8], [6 x i8]* @name.upper, i32 0, i32 0))
# IR: @llvmPy_print(%PyObj* [[upper]])

# OUTPUT: <PyFunc 0x{{[0-9A-F]+}}>
