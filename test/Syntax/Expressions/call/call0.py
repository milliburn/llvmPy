# RUN: test-ir.sh %s

# IR-LABEL: define %PyObj* @__body__

# IR: %callframe = alloca %Frame*

f = None
# IR: store %PyObj* @llvmPy_None, %PyObj** [[fPtr:%[^ ]+]]

f()
# IR: [[fPtr1:%[^ ]+]] = load %PyObj*, %PyObj** %var.f1
# IR-NEXT: [[fLabel:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[fPtr1]], i64 0)
# IR-NEXT: [[fFunc:%[^ ]+]] = bitcast i8* [[fLabel]] to %PyObj* (%Frame**)*
# IR-NEXT: [[fRV:%[^ ]+]] = call %PyObj* [[fFunc]](%Frame** %callframe)

# CHECK-DAG: declare i8* @llvmPy_fchk(%FrameN**, %PyObj*, i64)
