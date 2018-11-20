# RUN: %S/../test.sh %s

x = 2

if x == 1:
    print("A1")
elif x == 2:
    print("B1")
    print("B2")
else:
    print("C")
    print("C")

# CHECK-LABEL: B1
# CHECK-NEXT: B2
