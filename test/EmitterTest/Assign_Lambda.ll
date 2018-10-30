; INPUT: llvmPy.ir -c 'f = lambda: 1'

%PyObj = type opaque

define %PyObj* @__body__() prefix i64 //[0-9]+// {
  %1 = alloca %PyObj*
  store %PyObj* null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_func(%PyObj* ()* @lambda)
  store %PyObj* %2, %PyObj** %1
  ret %PyObj* null
}

define %PyObj* @lambda() prefix i64 //[0-9]+// {
  %1 = call %PyObj* @llvmPy_int(i64 1)
  ret %PyObj* %1
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_func(i64)
