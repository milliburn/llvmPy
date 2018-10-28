
%PyObj = type opaque

define void @__body__() {
  %1 = alloca %PyObj*
  store %PyObj** null, %PyObj** %1
  %2 = call %PyObj* @llvmPy_none()
  store %PyObj* %2, %PyObj** %1
  %3 = load %PyObj*, %PyObj** %1
  %2 = call %PyObj* @llvmPy_call0(%PyObj* %3)
}

declare %PyObj* @llvmPy_none()

declare %PyObj* @llvmPy_call0(%PyObj*)
