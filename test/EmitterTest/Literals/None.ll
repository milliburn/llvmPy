
%PyObj = type opaque

define void @__body__() {
  %1 = call %PyObj* @llvmPy_none()
}

declare %PyObj* @llvmPy_none()
