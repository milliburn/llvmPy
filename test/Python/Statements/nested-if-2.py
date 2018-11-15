# RUN: %S/../test.sh %s

print("Start")
x = 1
y = 2
if x == 1:
    if y == 2:
        print("A")
else:
    print("C")
print("D")

# CHECK-LABEL: Start
# CHECK-NEXT: A
# CHECK-NEXT: D
