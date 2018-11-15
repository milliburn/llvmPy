# RUN: %S/../test.sh %s

# CHECK-LABEL: define %PyObj* @__body__

x = 0

# CHECK-LABEL: while:
# CHECK: [[RESULT:%[a-z_0-9]+]] = call %PyObj* @llvmPy_lt
# CHECK-NEXT: [[TRUTHY:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT]])
# CHECK-NEXT: br i1 [[TRUTHY]], label %loop, label %endwhile
# CHECK-LABEL: loop:
while x < 10:
    print("Test")
    x = x + 1
    # CHECK: br label %while

# CHECK-LABEL: endwhile:
print(x)
# CHECK: @llvmPy_print(
# CHECK: ret %PyObj* @llvmPy_None
