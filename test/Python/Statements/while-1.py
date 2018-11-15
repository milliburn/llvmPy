# RUN: %S/../test.sh %s


print("test1:")
x = 0
while x < 3:
    print("Loop")
    print(x)
    x = x + 1
print("End")
print(x)

# CHECK-LABEL: test1:
# CHECK-NEXT: Loop
# CHECK-NEXT: 0
# CHECK-NEXT: Loop
# CHECK-NEXT: 1
# CHECK-NEXT: Loop
# CHECK-NEXT: 2
# CHECK-NEXT: End
# CHECK-NEXT: 3

