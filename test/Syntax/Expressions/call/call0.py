# RUN: llvmPy --ir %s > %t1
# RUN: cat -n %t1 >&2
# RUN: llvm-as < %t1 | llvm-dis | FileCheck %s

f = None
f()

# CHECK-LABEL: define %PyObj* @__body__
# CHECK: store %PyObj* null, %PyObj** [[VAR_PTR:%[a-z_0-9]+]]
# CHECK-NEXT: store %PyObj* @llvmPy_None, %PyObj** [[VAR_PTR]]
# CHECK-NEXT: [[VAR:%[a-z_0-9]+]] = load %PyObj*, %PyObj** [[VAR_PTR]]
# CHECK: %callframe = alloca %FrameN*
# CHECK: [[_FUNC:%[0-9]+]] = call i8* @llvmPy_fchk(%FrameN** %callframe, %PyObj* [[VAR]], i64 0)
# CHECK-NEXT: [[FUNC:%[0-9]+]] = bitcast i8* [[_FUNC]] to %PyObj* (%FrameN**)
# CHECK-NEXT: {{%[0-9]+}} = call %PyObj* [[FUNC]](%FrameN** %callframe)

# CHECK-DAG: declare i8* @llvmPy_fchk(%FrameN**, %PyObj*, i64)
