; INPUT: llvmPy.ir --naked -c '1 + 2'

%PyObj = type opaque

define %PyObj* @__body__() prefix i64 //[0-9]+// {
  %1 = call %PyObj* @llvmPy_int(i64 1)
  %2 = call %PyObj* @llvmPy_int(i64 2)
  %3 = call %PyObj* @llvmPy_add(%PyObj* %1, %PyObj* %2)
  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
