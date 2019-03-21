# RUN: test-ir.sh %s

# IR-DAG: %Frame = type opaque
# IR-DAG: %Frame.__body__ = type <{ %Frame.__body__*, %Frame*, [3 x %PyObj*] }>
# IR-DAG: %Frame.first = type <{ %Frame.first*, %Frame.__body__*, [0 x %PyObj*] }>
# IR-DAG: %Frame.second = type <{ %Frame.second*, %Frame.__body__*, [1 x %PyObj*] }>
# IR-DAG: %Frame.third = type <{ %Frame.third*, %Frame.__body__*, [2 x %PyObj*] }>
# IR-DAG: %Frame.fourth = type <{ %Frame.fourth*, %Frame.third*, [1 x %PyObj*] }>

# IR-LABEL: define %PyObj* @__body__
# IR: alloca %Frame.__body__

# IR-LABEL: define %PyObj* @first(%Frame.__body__* %outer)
# IR: alloca %Frame.first
def first():
    return 1 + 2


# IR-LABEL: define %PyObj* @second(%Frame.__body__* %outer, %PyObj* %arg.x)
# IR: alloca %Frame.second
def second(x):
    return 2 * x


# IR-LABEL: define %PyObj* @third(%Frame.__body__* %outer, %PyObj* %arg.x)
# IR: alloca %Frame.third
def third(x):
    def fourth(y):
        return x + y
    return fourth


# IR-LABEL: define %PyObj* @fourth(%Frame.third* %outer, %PyObj* %arg.y)
# IR: alloca %Frame.fourth
