# RUN: test-lexer.sh %s
# RUN: test-parser.sh %s
# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print("test".upper().capitalize())

# LEXER: >0 print ( "test" . upper ( ) . capitalize ( ) )

# PARSER:print("test".upper().capitalize())

# IR-LABEL: @__body__
# IR: [[str:%[^ ]+]] = load %PyObj*, %PyObj** @PyStr.0
# IR: [[upper:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[str]], %PyObj* %PyStr.1)
# IR: [[upperFunc_:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[upper]], i64 0)
# IR: [[upperFunc:%[^ ]+]] = bitcast i8* [[upperFunc_]] to %PyObj* (%Frame**)*
# IR: [[upperVal:%[^ ]+]] = call %PyObj* [[upperFunc]](%Frame** %callframe)
# IR: [[capitalize:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[upperVal]], %PyObj* %PyStr.2)
# IR: [[capitalizeFunc_:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe1, %PyObj* [[capitalize]], i64 0)
# IR: [[capitalizeFunc:%[^ ]+]] = bitcast i8* [[capitalizeFunc_]] to %PyObj* (%Frame**)*
# IR: [[capitalizeVal:%[^ ]+]] = call %PyObj* [[capitalizeFunc]](%Frame** %callframe1)
# IR: @llvmPy_print(%PyObj* [[capitalizeVal]])

# IR-LABEL: define @llvmPy_lmcall0(%Frame** %outerptr) {
# IR-NEXT:      %outer = load %Frame*, %Frame** %outerptr
# IR-NEXT:      %desc = bitcast %Frame* %outer to %Call*
# IR-NEXT:      %1 = getelementptr %Call, %Call* %desc, i64 0, i32 0
# IR-NEXT:      %label = load i8*, i8** %1
# IR-NEXT:      %2 = getelementptr %Call, %Call* %desc, i64 0, i32 1
# IR-NEXT:      %self = load %PyObj*, %PyObj** %2
# IR-NEXT:      %3 = bitcast i8* %label to %PyObj* (%PyObj*)*
# IR-NEXT:      %4 = call %PyObj* %label(%PyObj* %self)
# IR-NEXT:      ret %PyObj* %4
# IR-NEXT:  }

# OUTPUT: Test
