# RUN: %S/../test.sh %s


test_case = 1                       # CHECK-LABEL:test_case = 1
if True:                            #  CHECK-NEXT:if True:
    print("Test")                   #  CHECK-NEXT:    print("Test")


test_case = 2                       # CHECK-LABEL:test_case = 2
if x >= 1:                          #  CHECK-NEXT:if x >= 1:
    print("Test 1")                 #  CHECK-NEXT:    print("Test 1")
    print("Test 2")                 #  CHECK-NEXT:    print("Test 2")

test_case = 3                       # CHECK-LABEL:test_case = 3
if False:                           #  CHECK-NEXT:if False:
    print("False")                  #  CHECK-NEXT:    print("False")
else:                               #  CHECK-NEXT:else:
    print("True")                   #  CHECK-NEXT:    print("True")
