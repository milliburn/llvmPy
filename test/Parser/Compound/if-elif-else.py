# RUN: %S/../test.sh %s


test_case = 1                       # CHECK-LABEL:test_case = 1i
if True:                            #  CHECK-NEXT:if True:
    print("Test")                   #  CHECK-NEXT:    print("Test")


test_case = 2                       # CHECK-LABEL:test_case = 2i
if x >= 1:                          #  CHECK-NEXT:if (x >= 1i):
    print("Test 1")                 #  CHECK-NEXT:    print("Test 1")
    print("Test 2")                 #  CHECK-NEXT:    print("Test 2")

test_case = 3                       # CHECK-LABEL:test_case = 3i
if False:                           #  CHECK-NEXT:if False:
    print("False")                  #  CHECK-NEXT:    print("False")
else:                               #  CHECK-NEXT:else:
    print("True")                   #  CHECK-NEXT:    print("True")
