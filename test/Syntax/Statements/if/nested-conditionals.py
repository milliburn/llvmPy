# RUN: test-parser.sh %s
# RUN: test-output.sh %s

x = 1                   # PARSER-LABEL:x = 1i
y = 2                   #  PARSER-NEXT:y = 2i
print("Start")          #  PARSER-NEXT:print("Start")

# OUTPUT-LABEL: Start

if x == 1:              #  PARSER-NEXT:if (x == 1i):
    if y == 3:          #  PARSER-NEXT:    if (y == 3i):
        print("A")      #  PARSER-NEXT:        print("A")
else:                   #  PARSER-NEXT:else:
    print("C")          #  PARSER-NEXT:    print("C")
print("D")              #  PARSER-NEXT:print("D")

# OUTPUT-NEXT: D

if x == 1:              #  PARSER-NEXT:if (x == 1i):
    if y == 3:          #  PARSER-NEXT:    if (y == 3i):
        print("A")      #  PARSER-NEXT:        print("A")
    else:               #  PARSER-NEXT:    else:
        print("B")      #  PARSER-NEXT:        print("B")
else:                   #  PARSER-NEXT:else:
    print("C")          #  PARSER-NEXT:    print("C")
print("D")              #  PARSER-NEXT:print("D")

# OUTPUT-NEXT: B
# OUTPUT-NEXT: D

if x == 1:              #  PARSER-NEXT:if (x == 1i):
    if y == 3:          #  PARSER-NEXT:    if (y == 3i):
        print("A")      #  PARSER-NEXT:        print("A")
    else:               #  PARSER-NEXT:    else:
        print("B")      #  PARSER-NEXT:        print("B")
print("D")              #  PARSER-NEXT:print("D")

# OUTPUT-NEXT: B
# OUTPUT-NEXT: D

if x == 1:              #  PARSER-NEXT:if (x == 1i):
    if y == 3:          #  PARSER-NEXT:    if (y == 3i):
        print("A")      #  PARSER-NEXT:        print("A")
    else:               #  PARSER-NEXT:    else:
        print("X")      #  PARSER-NEXT:        print("X")
        if y == 2:      #  PARSER-NEXT:        if (y == 2i):
            print("B")  #  PARSER-NEXT:            print("B")
        else:           #  PARSER-NEXT:        else:
            print("E")  #  PARSER-NEXT:            print("E")
else:                   #  PARSER-NEXT:else:
    print("C")          #  PARSER-NEXT:    print("C")
print("D")              #  PARSER-NEXT:print("D")

# OUTPUT-NEXT: X
# OUTPUT-NEXT: B
# OUTPUT-NEXT: D
