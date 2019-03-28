# RUN: test-ir.sh %s

# IR-DAG: @llvmPy_tuple0 = external constant %PyObj
# IR: define
# IR-SAME: @__body__
print(())
# IR: call %PyObj* @llvmPy_print(%PyObj* @llvmPy_tuple0)
