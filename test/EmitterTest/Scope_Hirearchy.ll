; INPUT: llvmPy.ir -c 'x = 1; f = lambda: x + 1'

%PyObj = type opaque
%FrameN = type opaque
%Frame1 = type <{ %Frame1*, %FrameN*, [1 x %PyObj*] }>
%Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>

define %PyObj* @__body__(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame1
  %1 = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 0
  store %Frame1* %frame, %Frame1** %1
  %2 = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 1
  store %FrameN* %outer, %FrameN** %2
  %3 = getelementptr %Frame1, %Frame1* %frame, i64 0, i32 2, i64 0
  store %PyObj* null, %PyObj** %3

  %4 = call %PyObj* @llvmPy_int(i64 1)
  store %PyObj* %4, %PyObj** %3

  %5 = call %PyObj* @llvmPy_func(%Frame1* %frame, %PyObj* (%FrameN*)* @lambda)
  store %PyObj* %5, %PyObj** %3

  ret %PyObj* null
}

define %PyObj* @lambda(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame0
  %1 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 0
  store %Frame0* %frame, %Frame0** %1
  %2 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 1
  store %FrameN* %outer, %FrameN** %2

  %3 = getelementptr %Frame1, %FrameN* %outer, i64 0, i32 2, i64 0
  %4 = load %PyObj*, %PyObj** %3

  %5 = call %PyObj* @llvmPy_int(i64 1)
  %6 = call %PyObj* @llvmPy_add(%PyObj* %4, %PyObj* %5)

  ret %PyObj* %6
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_func(%FrameN*, i64)
declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)

