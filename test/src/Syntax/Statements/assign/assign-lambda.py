# Test that the assign statement reloads the frame pointer after a call to
# llvmPy_func(), which may lift the frame from the stack to the heap.


# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__
# IR: alloca [[F:%Frame.__body__]]

# IR: [[framePtrPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* %frame, i64 0, i32 0

f = lambda: 1

# IR: [[PyFunc:%[^ ]+]] = call %PyObj* @llvmPy_func
# IR-NEXT: [[framePtr:%[^ ]+]] = load [[F]]*, [[F]]** [[framePtrPtr]]
# IR-NEXT: [[fPtr:%[^ ]+]] = getelementptr [[F]], [[F]]* [[framePtr]], i64 0, i32 2, i64 0
# IR-NEXT: store %PyObj* [[PyFunc]], %PyObj** [[fPtr]]

# IR-LABEL: define %PyObj* @__lambda__{{[0-9]+}}
