# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__
# IR: alloca [[F:%Frame.__body__]]

x = 1

# IR: [[xPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* {{%[^ ]+}}, i64 0, i32 2, i64 [[xIdx:[0-9]+]]
# IR: store %PyObj* %PyInt.1, %PyObj** [[xPtr]]

f = lambda: x + 2

# IR-LABEL: define %PyObj* @__lambda__{{[0-9]+}}
# IR: load %Frame.__lambda__{{[0-9]+}}
# IR: [[outerPtr:%[^ ]+]] = load [[F]]*, [[F]]** {{%[^ ]+}}
# IR-NEXT: [[xPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* [[outerPtr]], i64 0, i32 2, i64 [[xIdx]]
# IR-NEXT: [[x:%[^ ]+]] = load %PyObj*, %PyObj** [[xPtr]]
# IR: call %PyObj* @llvmPy_add(%PyObj* [[x]]
