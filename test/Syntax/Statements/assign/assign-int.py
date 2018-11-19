# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__
# IR: alloca [[F:%Frame.__body__]]
# IR: [[framePtrPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* %frame, i64 0, i32 0

x = 5

# IR: %PyInt.5 = load %PyObj*, %PyObj** @PyInt.5
# IR-NEXT: [[framePtr:%[^ ]+]] = load [[F]]*, [[F]]** [[framePtrPtr]]
# IR-NEXT: [[xPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* [[framePtr]], i64 0, i32 2, i64 0
# IR-NEXT: store %PyObj* %PyInt.5, %PyObj** [[xPtr]]
