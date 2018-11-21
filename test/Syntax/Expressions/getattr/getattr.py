# RUN: test-lexer.sh %s
# RUN: test-parser.sh %s
# RUN: test-ir.sh %s
# RUN: test-output.sh %s

print("test".upper().capitalize())

# LEXER: >0 print ( "test" . upper ( ) . capitalize ( ) )

# PARSER:print("test".upper().capitalize())

# IR-DAG: @name.upper = private constant i8*
# IR-LABEL: @__body__
# IR: [[str:%[^ ]+]] = load %PyObj*, %PyObj** @PyStr.0
# IR-NEXT: [[upper:%[^ ]+]] = call %PyObj* getattr(%PyObj* [[str]], i8* @name.upper)
# IR-NEXT: %callframe = alloca %Frame*
# IR-NEXT: [[upperFunc:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[upper]], i64 1)
# IR-NEXT: [[upperVal:%[^ ]+]] = call %PyObj* [[upperFunc]](%Frame** %callframe, %PyObj* [[str]])
# IR-NEXT: [[capitalize:%[^ ]+]] = call %PyObj* getattr(%PyObj* [[upperVal]], i8* @name.capitalize)
# IR-NEXT: [[capitalizeFunc:%[^ ]+]] = call i8* @llvmPy_fchk(%Frame** %callframe, %PyObj* [[upperVal]], i64 1)
# IR-NEXT: [[capitalizeVal:%[^ ]+]] = call [[capitalizeFunc]](%Frame** %callframe, %PyObj* [[upperVal]])
# IR-NEXT: @llvmPy_print(%PyObj* [[capitalizeVal]])

# OUTPUT: "Test"
