# RUN: test-lexer.sh %s

test_case = 1               # LEXER-LABEL: >0 test_case = 1n
if x == 1:                  #  LEXER-NEXT: >0 >if x == 1n :
    pass                    #  LEXER-NEXT: >4 >pass
else:                       #  LEXER-NEXT: >0 >else :
    x = 2                   #  LEXER-NEXT: >4 x = 2n
                            #  LEXER-NEXT: >EOF
