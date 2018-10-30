; INPUT: llvmPy.ir --naked -c 'None'

%PyObj = type opaque

define %PyObj* @__body__() prefix i64 //[0-9]+// {
  %1 = call %PyObj* @llvmPy_none()
  ret %PyObj* null
}

declare %PyObj* @llvmPy_none()
