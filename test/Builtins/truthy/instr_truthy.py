# RUN: test-ir.sh %s

x = 2

# IR-LABEL: if.0:
# IR: [[BOOL:%[a-z_0-9]+]] = call %PyObj* @llvmPy_eq
# IR-NEXT: [[COND:%[0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[BOOL]])
# IR-NEXT: br i1 [[COND]], label %then.0, label %else.0
# IR-LABEL: then.0:
if x == 1:
    print("Yes")
else:
    print("No")

# IR-DAG: declare i1 @llvmPy_truthy(%PyObj*)
