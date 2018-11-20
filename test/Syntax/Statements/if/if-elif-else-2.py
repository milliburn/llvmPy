# RUN: %S/../test.sh %s

x = 1

print("Test Case 1")

if x == 1:
    print("A")

print("D")

# CHECK-LABEL: Test Case 1
# CHECK-NEXT: A
# CHECK-NEXT: D

print("Test Case 2")

if x != 1:
    print("A")

print("D")

# CHECK-LABEL: Test Case 2
# CHECK-NEXT: D

print("Test Case 3")

if x == 1:
    print("A")
else:
    print("B")

print("D")

# CHECK-LABEL: Test Case 3
# CHECK-NEXT: A
# CHECK-NEXT: D

print("Test Case 4")

if x > 1:
    print("A")
else:
    print("B")

print("D")

# CHECK-LABEL: Test Case 4
# CHECK-NEXT: B
# CHECK-NEXT: D

print("Test Case 5")

if x == 1:
    print("A")
elif x == 2:
    print("B")
else:
    print("C")

print("D")

# CHECK-LABEL: Test Case 5
# CHECK-NEXT: A
# CHECK-NEXT: D

print("Test Case 6")

if x == 1:
    print("A")
elif x == 1:
    print("B")
else:
    print("C")

print("D")

# CHECK-LABEL: Test Case 6
# CHECK-NEXT: A
# CHECK-NEXT: D

print("Test Case 7")

if x != 1:
    print("A")
elif x == 1:
    print("B")
else:
    print("C")

print("D")

# CHECK-LABEL: Test Case 7
# CHECK-NEXT: B
# CHECK-NEXT: D

print("Test Case 8")

if x != 1:
    print("A")
elif x != 1:
    print("B")
else:
    print("C")

print("D")

# CHECK-LABEL: Test Case 8
# CHECK-NEXT: C
# CHECK-NEXT: D

print("Test Case 9")

if x != 1:
    print("A")
elif x != 1:
    print("B")

print("D")

# CHECK-LABEL: Test Case 9
# CHECK-NEXT: D
