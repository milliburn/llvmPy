# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__

x = 0

# IR-LABEL: while:
# IR: [[RESULT:%[a-z_0-9]+]] = call %PyObj* @llvmPy_lt
# IR-NEXT: [[TRUTHY:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT]])
# IR-NEXT: br i1 [[TRUTHY]], label %loop, label %endwhile
# IR-LABEL: loop:
while x < 10:
    print("Test")
    x = x + 1
    # IR: br label %while

# IR-LABEL: endwhile:
print(x)
# IR: @llvmPy_print(
