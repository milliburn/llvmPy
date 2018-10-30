; INPUT: llvmPy.ir --naked -c "foo = None; foo(1)"

%PyObj = type opaque

define void @__body__() {
  %1 = alloca %PyObj*
  store %PyObj** null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_none()
  store %PyObj* %2, %PyObj** %1
  %3 = load %PyObj*, %PyObj** %1
  %4 = call %PyObj* @llvmPy_int(i64 1)
  %5 = call %PyObj* @llvmPy_call1(%PyObj* %3, %PyObj* %4)
}

declare %PyObj* @llvmPy_none()
declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_call1(%PyObj*, %PyObj*)
