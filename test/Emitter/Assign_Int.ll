; RUN: llvm-as %s | llvm-dis > %t1
; RUN: llvmPy --ir -c 'x = 1' | llvm-as | llvm-dis > %t2
; RUN: diff %t1 %t2

%PyObj = type opaque
%FrameN = type opaque
%Frame1 = type <{ %Frame1*, %FrameN*, [1 x %PyObj*] }>

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

  ret %PyObj* null
}

declare %PyObj* @llvmPy_int(i64)
