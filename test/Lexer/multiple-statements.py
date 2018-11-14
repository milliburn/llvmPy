# RUN: %S/test.sh %s


x = 1 + 2               # CHECK-LABEL: >0 x = 1n + 2n
print(foo(x))           #  CHECK-NEXT: >0 print ( foo ( x ) )
                        #  CHECK-NEXT: >EOF
