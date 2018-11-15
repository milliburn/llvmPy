# RUN: %S/../test.sh %s


# CHECK-LABEL: define %PyObj* @__body__

x = 0

# CHECK: [[RESULT:%[a-z_0-9]+]] = call %PyObj* @llvmPy_eq
# CHECK-NEXT: [[TRUTHY:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT]])
# CHECK-NEXT: br i1 [[TRUTHY]], label %then, label %else
# CHECK-LABEL: then:
if x == 1:
    x = 10
    # CHECK: br label %[[OUT:[a-z_0-9]+]]
# CHECK-LABEL: else:
else:
    x = 11
    # CHECK: br label %[[OUT]]

# CHECK: [[OUT]]:
print(x)
# CHECK: @llvmPy_print(
# CHECK: ret %PyObj* @llvmPy_None
