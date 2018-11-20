# RUN: test-output.sh %s

x = 2

print("Start")          # OUTPUT-LABEL: Start

if x == 2:
    print("A1")         # OUTPUT-NEXT: A1
elif x == 1:
    print("A2")
else:
    print("A3")

if x == 1:
    print("B1")
elif x == 2:
    print("B2")         # OUTPUT-NEXT: B2
else:
    print("B3")

if x == 0:
    print("C1")
elif x == 1:
    print("C2")
else:
    print("C3")         # OUTPUT-NEXT: C3
