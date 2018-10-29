; INPUT: llvmPy.ir --naked -c '1'

%PyObj = type opaque

define void @__body__() {
  %1 = call %PyObj* @llvmPy_int(i64 1)
}

declare %PyObj* @llvmPy_int(i64)
