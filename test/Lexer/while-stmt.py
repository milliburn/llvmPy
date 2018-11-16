# RUN: %S/test.sh %s


test_case = 1               # CHECK-LABEL: >0 test_case = 1n
x = 0                       #  CHECK-NEXT: >0 x = 0n
while x < 10:               #  CHECK-NEXT: >0 >while x < 10n :
    print("Test")           #  CHECK-NEXT: >4 print ( "Test" )
    x = x + 1               #  CHECK-NEXT: >4 x = x + 1n
    break                   #  CHECK-NEXT: >4 >break
    continue                #  CHECK-NEXT: >4 >continue
print(x)                    #  CHECK-NEXT: >0 print ( x )
                            #  CHECK-NEXT: >EOF
