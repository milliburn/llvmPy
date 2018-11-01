; INPUT: llvmPy.ir -c 'x = 1; f = lambda: x + 1'

%PyObj = type opaque
%FrameN = type opaque
%Frame2 = type <{ %Frame2*, %FrameN*, [2 x %PyObj*] }>
%Frame0 = type <{ %Frame0*, %FrameN*, [0 x %PyObj*] }>

define %PyObj* @__body__(%FrameN* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame2
  %1 = getelementptr %Frame2, %Frame2* %frame, i64 0, i32 0
  store %Frame2* %frame, %Frame2** %1
  %2 = getelementptr %Frame2, %Frame2* %frame, i64 0, i32 1
  store %FrameN* %outer, %FrameN** %2
  %3 = getelementptr %Frame2, %Frame2* %frame, i64 0, i32 2, i64 0
  store %PyObj* null, %PyObj** %3
  %4 = getelementptr %Frame2, %Frame2* %frame, i64 0, i32 2, i64 1
  store %PyObj* null, %PyObj** %4

  %5 = call %PyObj* @llvmPy_int(i64 1)
  store %PyObj* %5, %PyObj** %3

  %6 = call %PyObj* @llvmPy_func(%Frame2* %frame, %PyObj* (%Frame2*)* @lambda)
  store %PyObj* %6, %PyObj** %4

  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)

define %PyObj* @lambda(%Frame2* %outer) prefix i64 //[0-9]+// {
  %frame = alloca %Frame0
  %1 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 0
  store %Frame0* %frame, %Frame0** %1
  %2 = getelementptr %Frame0, %Frame0* %frame, i64 0, i32 1
  store %Frame2* %outer, %FrameN** %2

  %3 = getelementptr %Frame2, %Frame2* %outer, i64 0, i32 2, i64 0
  %4 = load %PyObj*, %PyObj** %3

  %5 = call %PyObj* @llvmPy_int(i64 1)
  %6 = call %PyObj* @llvmPy_add(%PyObj* %4, %PyObj* %5)

  ret %PyObj* %6
}

declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
declare %PyObj* @llvmPy_func(%FrameN*, i64)

