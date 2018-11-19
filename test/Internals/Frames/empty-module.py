# RUN: test-ir.sh %s

# IR-DAG: %PyObj = type opaque
# IR-DAG: %Frame = type opaque
# IR-DAG: %Frame.__body__ = type <{ %Frame.__body__*, %Frame*, [0 x %PyObj*] }>

# IR-LABEL: define %PyObj* @__body__(%Frame** %outerptr) prefix i64 {{[0-9]+}} {
#  IR-NEXT:     %frame = alloca %Frame.__body__
#  IR-NEXT:     %1 = getelementptr %Frame.__body__, %Frame.__body__* %frame, i64 0, i32 0
#  IR-NEXT:     %2 = getelementptr %Frame.__body__, %Frame.__body__* %frame, i64 0, i32 1
#  IR-NEXT:     store %Frame.__body__* %frame, %Frame.__body__** %1
#  IR-NEXT:     %outer = load %Frame*, %Frame** %outerptr
#  IR-NEXT:     store %Frame* %outer, %Frame** %2
#  IR-NEXT:     ret %PyObj* @llvmPy_None
#  IR-NEXT: }
