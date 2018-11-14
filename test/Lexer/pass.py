# RUN: %S/test.sh %s

test_case = 1               # CHECK-LABEL: >0 test_case = 1n
if x == 1:                  #  CHECK-NEXT: >0 >if x == 1n :
    pass                    #  CHECK-NEXT: >4 >pass
else:                       #  CHECK-NEXT: >0 >else :
    x = 2                   #  CHECK-NEXT: >4 x = 2n
                            #  CHECK-NEXT: >EOF
