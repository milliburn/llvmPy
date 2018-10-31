; INPUT: llvmPy.ir -c '1'

%PyObj = type opaque
%FrameN = type opaque
%Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>

define %PyObj* @__body__(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame0
  %1 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 0
  store %Frame0* %frame, %Frame0** %1
  %2 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 1
  store %FrameN* %outer, %FrameN** %2
  %3 = call %PyObj* @llvmPy_int(i64 1)
  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)
