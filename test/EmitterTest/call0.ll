; INPUT: llvmPy.ir --naked -c "foo = None; foo()"

%PyObj = type opaque

define void @__body__() {
  %1 = alloca %PyObj*
  store %PyObj** null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_none()
  store %PyObj* %2, %PyObj** %1
  %3 = load %PyObj*, %PyObj** %1
  %4 = call %PyObj* ()* @llvmPy_fchk(%PyObj* %3, i64 0)
  %5 = call %PyObj* %4()
}

declare %PyObj* @llvmPy_none()
declare %PyObj* ()* @llvmPy_fchk(%PyObj*, i64)
