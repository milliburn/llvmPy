# RUN: test-lexer.sh %s

# NOTE: "import" isn't a proper keyword yet.

import module1                  # LEXER-LABEL: >0 import module1
import module2                  #  LEXER-NEXT: >0 import module2

global_y = 1                    #  LEXER-NEXT: >0 global_y = 1n

def my_func(x):                 #  LEXER-NEXT: >0 >def my_func ( x ) :
    if x == 1:                  #  LEXER-NEXT: >4 >if x == 1n :
        return global_y + x     #  LEXER-NEXT: >8 >return global_y + x
    else:                       #  LEXER-NEXT: >4 >else :
        return 2                #  LEXER-NEXT: >8 >return 2n

if __name__ == '__main__':      #  LEXER-NEXT: >0 >if __name__ == '__main__' :
    print(global_y)             #  LEXER-NEXT: >4 print ( global_y )
                                #  LEXER-NEXT: >EOF
