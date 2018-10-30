; INPUT: llvmPy.ir -c '1'

%PyObj = type opaque

define %PyObj* @__body__() prefix i64 //[0-9]+// {
  %1 = call %PyObj* @llvmPy_int(i64 1)
  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)
