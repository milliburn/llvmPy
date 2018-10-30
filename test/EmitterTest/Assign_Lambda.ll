; INPUT: llvmPy.ir -c 'f = lambda: 1'

%PyObj = type opaque
%FrameN = type opaque
%Frame1 = type <{ %Frame1*, %FrameN*, [1 x %PyObj*] }>

define %PyObj* @__body__(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame1
  %1 = getelementptr %Frame1*, %Frame1* %frame, i32 0
  store %Frame1* %frame, %Frame1** %1
  %2 = getelementptr %FrameN*, %Frame1* %frame, i32 1
  store %FrameN* %outer, %FrameN** %2
  %3 = getelementptr %PyObj*, %Frame1* %frame, i32 2, i64 0
  store %PyObj* null, %PyObj** %3

  %4 = call %PyObj* @llvmPy_func(%Frame1* %frame, %PyObj* ()* @lambda)

  %5 = getelementptr %Frame1*, %Frame1* %frame, i32 0
  %6 = load %Frame1*, %Frame1** %5
  %7 = getelementptr %PyObj*, %Frame1* %6, i32 2, i64 0
  store %PyObj* %4, %PyObj** %7

  ret %PyObj* null
}

define %PyObj* @lambda(%FrameN* %outer) prefix i64 //[0-9]+// {
  %1 = call %PyObj* @llvmPy_int(i64 1)
  ret %PyObj* %1
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_func(%FrameN*, i64)
