# RUN: %S/../test.sh %s


# CHECK-LABEL: define %PyObj* @__body__

x = 0

# CHECK-LABEL: if:
# CHECK: [[RESULT:%[a-z_0-9]+]] = call %PyObj* @llvmPy_eq
# CHECK-NEXT: [[TRUTHY:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT]])
# CHECK-NEXT: br i1 [[TRUTHY]], label %then, label %else
if x == 1:
    # CHECK-LABEL: then:
    x = 10
# CHECK-LABEL: else:
# CHECK-NEXT: br label %if1
# CHECK-LABEL: if1:
# CHECK: [[RESULT1:%[a-z_0-9]+]] = call %PyObj* @llvmPy_ge
# CHECK-NEXT: [[TRUTHY1:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT1]])
# CHECK-NEXT: br i1 [[TRUTHY1]], label %then2, label %else3
elif x >= 2:
    # CHECK-LABEL: then2:
    x = 11
# CHECK-LABEL: else3:
# CHECK-NEXT: br label %if4
# CHECK-LABEL: if4:
# CHECK: [[RESULT2:%[a-z_0-9]+]] = call %PyObj* @llvmPy_ne
# CHECK-NEXT: [[TRUTHY2:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT2]])
# CHECK-NEXT: br i1 [[TRUTHY2]], label %then5, label %else6
elif x != 3:
    # CHECK-LABEL: then5:
    x = 12
# CHECK-LABEL: else6:
else:
    x = 13

# CHECK-LABEL: endif:
# CHECK-NEXT: br label %endif
# CHECK-LABEL: endif7:
# CHECK-NEXT: br label %endif8
# CHECK-LABEL: endif8:

print(x)

# CHECK: @llvmPy_print(
# CHECK-NEXT: ret %PyObj* @llvmPy_None
