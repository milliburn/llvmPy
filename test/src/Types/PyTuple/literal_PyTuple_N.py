# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print((1, 2, 3))
# IR: [[buffer:%buffer]] = call i64 @llvmPy_malloc(i64 24)
# IR-NEXT: [[array:%[0-9]+]] = inttoptr i64 %buffer to [3 x %PyObj*]*
# IR-NEXT: %PyInt.1 =
# IR-NEXT: [[ptr1:%[0-9]+]] = getelementptr [3 x %PyObj*], [3 x %PyObj*]* [[array]], i64 0, i64 0
# IR-NEXT: store %PyObj* %PyInt.1, %PyObj** [[ptr1]]
# IR: [[tuple:%[0-9]+]] = call %PyObj* @llvmPy_tupleN(i64 3, i64 [[buffer]])
# IR-NEXT: call %PyObj* @llvmPy_print(%PyObj* [[tuple]])

# OUTPUT: (1, 2, 3)
