# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

"Greetings, programs!"

# CHECK: @str = private unnamed_addr constant [22 x i8] c"Greetings, programs!\22\00"

# CHECK: define
# CHECK-SAME: @__body__

# CHECK: {{%[a-z_0-9]}} = call %PyObj* @llvmPy_str(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @str, i32 0, i32 0))

# CHECK-DAG: declare %PyObj* @llvmPy_str(i8*)

