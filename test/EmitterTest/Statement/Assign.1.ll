
%PyObj = type opaque

define void @__body__() {
  %1 = alloca %PyObj*
  store %PyObj** null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_int(i64 1)
  store %PyObj* %2, %PyObj** %1
}

declare %PyObj* @llvmPy_int(i64)
