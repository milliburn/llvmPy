# RUN: %S/../test.sh %s


# CHECK-LABEL: define %PyObj* @__body__

x = 0

# CHECK-LABEL: if0:
# CHECK: [[RESULT0:%[a-z_0-9]+]] = call %PyObj* @llvmPy_eq
# CHECK-NEXT: [[TRUTHY0:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT0]])
# CHECK-NEXT: br i1 [[TRUTHY0]], label if0_then, label if0_else
if x == 1:
    # CHECK-LABEL: if0_then:
    x = 10
# CHECK-LABEL: if0_else:
# CHECK-LABEL: if1:
# CHECK: [[RESULT1:%[a-z_0-9]+]] = call %PyObj* @llvmPy_ge
# CHECK-NEXT: [[TRUTHY1:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT1]])
# CHECK-NEXT: br i1 [[TRUTHY1]], label if1_then, label if1_else
elif x >= 2:
    # CHECK-LABEL: if1_then:
    x = 11
# CHECK-LABEL: if1_else:
# CHECK-LABEL: if2:
# CHECK: [[RESULT2:%[a-z_0-9]+]] = call %PyObj* @llvmPy_ne
# CHECK-NEXT: [[TRUTHY2:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT2]])
# CHECK-NEXT: br i1 [[TRUTHY2]], label if2_then, label if2_else
elif x != 3:
    # CHECK-LABEL: if2_then:
    x = 12
# CHECK-LABEL: if2_else:
# CHECK-NOT: if3
else:
    x = 13
