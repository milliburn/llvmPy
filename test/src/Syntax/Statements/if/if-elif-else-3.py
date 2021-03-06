# RUN: test-output.sh %s

x = 1

print("Test Case 1")

if x == 1:
    print("A")

print("D")

# OUTPUT-LABEL: Test Case 1
# OUTPUT-NEXT: A
# OUTPUT-NEXT: D

print("Test Case 2")

if x != 1:
    print("A")

print("D")

# OUTPUT-LABEL: Test Case 2
# OUTPUT-NEXT: D

print("Test Case 3")

if x == 1:
    print("A")
else:
    print("B")

print("D")

# OUTPUT-LABEL: Test Case 3
# OUTPUT-NEXT: A
# OUTPUT-NEXT: D

print("Test Case 4")

if x > 1:
    print("A")
else:
    print("B")

print("D")

# OUTPUT-LABEL: Test Case 4
# OUTPUT-NEXT: B
# OUTPUT-NEXT: D

print("Test Case 5")

if x == 1:
    print("A")
elif x == 2:
    print("B")
else:
    print("C")

print("D")

# OUTPUT-LABEL: Test Case 5
# OUTPUT-NEXT: A
# OUTPUT-NEXT: D

print("Test Case 6")

if x == 1:
    print("A")
elif x == 1:
    print("B")
else:
    print("C")

print("D")

# OUTPUT-LABEL: Test Case 6
# OUTPUT-NEXT: A
# OUTPUT-NEXT: D

print("Test Case 7")

if x != 1:
    print("A")
elif x == 1:
    print("B")
else:
    print("C")

print("D")

# OUTPUT-LABEL: Test Case 7
# OUTPUT-NEXT: B
# OUTPUT-NEXT: D

print("Test Case 8")

if x != 1:
    print("A")
elif x != 1:
    print("B")
else:
    print("C")

print("D")

# OUTPUT-LABEL: Test Case 8
# OUTPUT-NEXT: C
# OUTPUT-NEXT: D

print("Test Case 9")

if x != 1:
    print("A")
elif x != 1:
    print("B")

print("D")

# OUTPUT-LABEL: Test Case 9
# OUTPUT-NEXT: D
