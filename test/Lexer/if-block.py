# RUN: %S/test.sh %s


if x == 2:              # CHECK-LABEL: >0 if x == 2n :
    print(foo())        #  CHECK-NEXT: >4 print ( foo ( ) )
                        #  CHECK-NEXT: >EOF
