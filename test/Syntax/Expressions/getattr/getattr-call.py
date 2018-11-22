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
# IR: [[capitalizeFunc_:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[capitalize]], i64 0)
# IR: [[capitalizeFunc:%[^ ]+]] = bitcast i8* [[capitalizeFunc_]] to %PyObj* (%Frame**)*
# IR: [[capitalizeVal:%[^ ]+]] = call %PyObj* [[capitalizeFunc]](%Frame** %callframe)
# IR: @llvmPy_print(%PyObj* [[capitalizeVal]])

# OUTPUT: Test
