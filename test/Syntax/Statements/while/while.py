# RUN: test-lexer.sh %s
# RUN: test-parser.sh %s
# RUN: test-ir.sh %s
# RUN: test-output.sh %s

# IR-LABEL: define %PyObj* @__body__

i = 0
print("Start: " + str(i))

# IR-LABEL: while:
# IR: [[RESULT:%[a-z_0-9]+]] = call %PyObj* @llvmPy_lt
# IR-NEXT: [[TRUTHY:%[a-z_0-9]+]] = call i1 @llvmPy_truthy(%PyObj* [[RESULT]])
# IR-NEXT: br i1 [[TRUTHY]], label %loop, label %endwhile
# IR-LABEL: loop:
while i < 4:
    print("Loop " + str(i))
    i = i + 1
    # IR: br label %while

# IR-LABEL: endwhile:
print("End: " + str(i))

# LEXER-LABEL: >0 i = 0n
#  LEXER-NEXT: >0 print ( "Start: " + str ( i ) )
#  LEXER-NEXT: >0 >while i < 4n :
#  LEXER-NEXT: >4 print ( "Loop " + str ( i ) )
#  LEXER-NEXT: >4 i = i + 1n
#  LEXER-NEXT: >0 print ( "End: " + str ( i ) )
#  LEXER-NEXT: >EOF

# PARSER-LABEL:i = 0i
#  PARSER-NEXT:print(("Start: " + str(i)))
#  PARSER-NEXT:while (i < 4i):
#  PARSER-NEXT:    print(("Loop " + str(i)))
#  PARSER-NEXT:    i = (i + 1i)
#  PARSER-NEXT:print(("End: " + str(i)))

# OUTPUT-LABEL: Start: 0
# OUTPUT-NEXT: Loop 0
# OUTPUT-NEXT: Loop 1
# OUTPUT-NEXT: Loop 2
# OUTPUT-NEXT: Loop 3
# OUTPUT-NEXT: End: 4
