# RUN: test-lexer.sh %s


x = 1 + 2               # LEXER-LABEL: >0 x = 1n + 2n
print(foo(x))           #  LEXER-NEXT: >0 print ( foo ( x ) )
                        #  LEXER-NEXT: >EOF
