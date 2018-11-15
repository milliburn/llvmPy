# RUN: %S/../test.sh %s

x = 2

if x == 1:
    print("A")
elif x == 2:
    print("B")
else:
    print("C")

# CHECK: B
