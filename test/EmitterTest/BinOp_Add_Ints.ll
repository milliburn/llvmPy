; INPUT: llvmPy.ir --naked -c '1 + 2'

%PyObj = type opaque

define void @__body__() {
  %1 = call %PyObj* @llvmPy_int(i64 1)
  %2 = call %PyObj* @llvmPy_int(i64 2)
  %3 = call %PyObj* @llvmPy_add(%PyObj* %1, %PyObj* %2)
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
