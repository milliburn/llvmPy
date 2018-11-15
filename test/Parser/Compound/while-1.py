# RUN: %S/../test.sh %s

test_case = 1               # CHECK-LABEL:test_case = 1i
x = 0                       #  CHECK-NEXT:x = 0i
while x < 10:               #  CHECK-NEXT:while (x < 10i):
    print("Test")           #  CHECK-NEXT:    print("Test")
    x = x + 1               #  CHECK-NEXT:    x = (x + 1i)
print(x)                    #  CHECK-NEXT:print(x)
