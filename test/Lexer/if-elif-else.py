# RUN: %S/test.sh %s


if x == 2:              # CHECK-LABEL: >0 >if x == 2n :
    print(foo())        #  CHECK-NEXT: >4 print ( foo ( ) )
elif x == 3:            #  CHECK-NEXT: >0 >elif x == 3n :
    print(bar())        #  CHECK-NEXT: >4 print ( bar ( ) )
    print(bax())        #  CHECK-NEXT: >4 print ( bax ( ) )
else:                   #  CHECK-NEXT: >0 >else :
    print(baz())        #  CHECK-NEXT: >4 print ( baz ( ) )
                        #  CHECK-NEXT: >EOF
