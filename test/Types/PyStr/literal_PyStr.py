# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# IR: @PyStr.0 = private constant %PyObj* inttoptr (i64 {{[0-9]+}} to %PyObj*)

print("Greetings, programs!")
# IR: %PyStr.0 = load %PyObj*, %PyObj** @PyStr.0
# IR-NEXT: call %PyObj* @llvmPy_print(%PyObj* %PyStr.0)

# OUTPUT: Greetings, programs!
