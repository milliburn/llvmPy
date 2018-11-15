# RUN: %S/../test.sh %s

# NOTE: "import" isn't a proper keyword yet.

import module1                  # CHECK-LABEL: >0 import module1
import module2                  #  CHECK-NEXT: >0 import module2

global_y = 1                    #  CHECK-NEXT: >0 global_y = 1n

def my_func(x):                 #  CHECK-NEXT: >0 >def my_func ( x ) :
    if x == 1:                  #  CHECK-NEXT: >4 >if x == 1n :
        return global_y + x     #  CHECK-NEXT: >8 >return global_y + x
    else:                       #  CHECK-NEXT: >4 >else :
        return 2                #  CHECK-NEXT: >8 >return 2n

if __name__ == '__main__':      #  CHECK-NEXT: >0 >if __name__ == '__main__' :
    print(global_y)             #  CHECK-NEXT: >4 print ( global_y )
                                #  CHECK-NEXT: >EOF
