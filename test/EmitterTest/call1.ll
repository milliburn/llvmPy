; INPUT: llvmPy.ir -c "foo = None; foo(1)"

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

  %4 = call %PyObj* @llvmPy_none()
  store %PyObj* %4, %PyObj** %3

  %5 = call %PyObj* @llvmPy_int(i64 1)
  %callframe = alloca %FrameN*
  %6 = call %PyObj* ()* @llvmPy_fchk(%FrameN** %callframe, %PyObj* %5, i64 1)
  %7 = call %PyObj* %fp(%FrameN** %callframe, %PyObj* %6)

  ret %PyObj* null
}

declare %PyObj* @llvmPy_none()
declare %PyObj* @llvmPy_int(i64)
declare %PyObj* ()* @llvmPy_fchk(%FrameN**, %PyObj*, i64)
