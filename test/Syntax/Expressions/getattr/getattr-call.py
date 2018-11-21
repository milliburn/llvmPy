# RUN: test-lexer.sh %s
# RUN: test-parser.sh %s
# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print("test".upper().capitalize())

# LEXER: >0 print ( "test" . upper ( ) . capitalize ( ) )

# PARSER:print("test".upper().capitalize())

# IR-DAG: @name.upper = private unnamed_addr constant [6 x i8] c"upper\00"
# IR-DAG: @name.capitalize = private unnamed_addr constant [11 x i8] c"capitalize\00"
# IR-LABEL: @__body__
# IR: [[str:%[^ ]+]] = load %PyObj*, %PyObj** @PyStr.0
# IR: [[upper:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[str]], i8* getelementptr inbounds ([6 x i8], [6 x i8]* @name.upper, i32 0, i32 0))
# IR: [[upperFunc_:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[upper]], i64 1)
# IR: [[upperFunc:%[^ ]+]] = bitcast i8* [[upperFunc_]] to %PyObj* (%Frame**, %PyObj*)*
# IR: [[upperVal:%[^ ]+]] = call %PyObj* [[upperFunc]](%Frame** %callframe, %PyObj* [[str]])
# IR: [[capitalize:%[^ ]+]] = call %PyObj* @llvmPy_getattr(%PyObj* [[upperVal]], i8* getelementptr inbounds ([11 x i8], [11 x i8]* @name.capitalize, i32 0, i32 0))
# IR: [[capitalizeFunc_:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe1, %PyObj* [[capitalize]], i64 1)
# IR: [[capitalizeFunc:%[^ ]+]] = bitcast i8* [[capitalizeFunc_]] to %PyObj* (%Frame**, %PyObj*)*
# IR: [[capitalizeVal:%[^ ]+]] = call %PyObj* [[capitalizeFunc]](%Frame** %callframe1, %PyObj* [[upperVal]])
# IR: @llvmPy_print(%PyObj* [[capitalizeVal]])

# OUTPUT: Test
