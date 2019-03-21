# RUN: test-lexer.sh %s


if x == 2:              # LEXER-LABEL: >0 >if x == 2n :
    print(foo())        #  LEXER-NEXT: >4 print ( foo ( ) )
elif x == 3:            #  LEXER-NEXT: >0 >elif x == 3n :
    print(bar())        #  LEXER-NEXT: >4 print ( bar ( ) )
    print(bax())        #  LEXER-NEXT: >4 print ( bax ( ) )
else:                   #  LEXER-NEXT: >0 >else :
    print(baz())        #  LEXER-NEXT: >4 print ( baz ( ) )
                        #  LEXER-NEXT: >EOF
