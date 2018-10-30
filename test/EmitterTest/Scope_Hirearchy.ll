; IGNORE
; INPUT: llvmPy.ir --naked -c 'y = 1; f = lambda x: x + y'

%PyObj = type opaque

define void @__body__() {
    ;; Allocate and initialise scoped variable "y".

    %v_y = alloca %PyObj*
    store %PyObj** null, %PyObj** %v_y
    %1 = call %PyObj* @llvmPy_int(i64 1)
    store %PyObj* %1, %PyObj** %v_y

    ;; Allocate and initialise scoped variable "f".

    %v_f = alloca %PyObj*
    store %PyObj** null, %PyObj** %v_f
    %2 = call %PyObj* @llvmPy_func(%PyObj* (%PyObj*)* @lambda)
    store %PyObj* %2, %PyObj** %v_f
}

;; Define a function that takes one argument.

define %PyObj* @lambda(%PyObj* %p_x) prefix i64 //[0-9]+// {

    ;; Acquire value of slot "y" from an outer scope.

    %v_y = call %PyObj* @llvmPy_get()  ; TODO: Pass pointer to string "y".

    ;; Call addition operator.

    %1 = call %PyObj* @llvmPy_add(%PyObj* %p_x, %PyObj* %v_y)

    ;; Return with result of addition.

    ret %PyObj* %1

}

declare %PyObj* @llvmPy_int(i64)
declare %PyObj* @llvmPy_func(i64)
declare %PyObj* @llvmPy_add(%PyObj*, %PyObj*)
declare %PyObj* @llvmPy_get(i8*)
