# RUN: %S/../test.sh %s

x = 1                   # CHECK-LABEL:x = 1i
y = 2                   #  CHECK-NEXT:y = 2i
if x == 1:              #  CHECK-NEXT:if (x == 1i):
    if y == 3:          #  CHECK-NEXT:    if (y == 3i):
        print("A")      #  CHECK-NEXT:        print("A")
    else:               #  CHECK-NEXT:    else:
        print("B")      #  CHECK-NEXT:        print("B")
else:                   #  CHECK-NEXT:else:
    print("C")          #  CHECK-NEXT:    print("C")
print("D")              #  CHECK-NEXT:print("D")
