# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# IR: @__body__
x = 1, 2
y = x[0]
# IR: %var.x1 =
# IR-NEXT: [[x:%[0-9]+]] = load %PyObj*, %PyObj** %var.x1
# IR-NEXT: %PyInt.0 =
# IR-NEXT: call %PyObj* @llvmPy_getitem(%PyObj* [[x]], %PyObj* %PyInt.0)

print(y)
# OUTPUT: 1

print(x[1])
# OUTPUT-NEXT: 2
