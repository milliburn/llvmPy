# RUN: %S/../test.sh %s

print("Start")
x = 1
y = 2
if x == 1:
    if y == 3:
        print("A")
    else:
        print("B")
else:
    print("C")
print("D")

# CHECK-LABEL: Start
# CHECK-NEXT: B
# CHECK-NEXT: D
