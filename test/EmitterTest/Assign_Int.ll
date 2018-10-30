; INPUT: llvmPy.ir --naked -c 'x = 1'

%PyObj = type opaque

define %PyObj* @__body__() prefix i64 //[0-9]+// {
  %1 = alloca %PyObj*
  store %PyObj* null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_int(i64 1)
  store %PyObj* %2, %PyObj** %1
  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)
