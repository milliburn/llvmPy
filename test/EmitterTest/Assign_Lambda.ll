; INPUT: llvmPy.ir -c 'f = lambda: 1'

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

  ; XXX: Why does it emit %Frame0* here?
  %4 = call %PyObj* @llvmPy_func(%Frame0* %frame, %PyObj* (%FrameN*)* @lambda)

  store %PyObj* %4, %PyObj** %3

  ret %PyObj* null
}

define %PyObj* @lambda(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame0
  %1 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 0
  store %Frame0* %frame, %Frame0** %1
  %2 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 1
  store %FrameN* %outer, %FrameN** %2
  %3 = call %PyObj* @llvmPy_int(i64 1)
  ret %PyObj* %3
}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_func(%FrameN*, i64)
