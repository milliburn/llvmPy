# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__
# IR: %callframe = alloca %Frame*

f = None
# IR: store %PyObj* @llvmPy_None, %PyObj** [[fPtr:%[^ ]+]]

f(1)
# IR: [[fPtr1:%[^ ]+]] = load %PyObj*, %PyObj** %var.f1
# IR: [[fLabel:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[fPtr1]], i64 1)
# IR-NEXT: [[fFunc:%[^ ]+]] = bitcast i8* [[fLabel]] to %PyObj* (%Frame**, %PyObj*)*
# IR-NEXT: [[fRV:%[^ ]+]] = call %PyObj* [[fFunc]](%Frame** %callframe, %PyObj* %PyInt.1)

# IR-DAG: declare i8* @llvmPy_fchk(%Frame**, %PyObj*, i64)
