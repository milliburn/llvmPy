# RUN: test-parser.sh %s

test_case = 1               # PARSER-LABEL:test_case = 1i
x = 0                       #  PARSER-NEXT:x = 0i
while x < 10:               #  PARSER-NEXT:while (x < 10i):
    print("Test")           #  PARSER-NEXT:    print("Test")
    x = x + 1               #  PARSER-NEXT:    x = (x + 1i)
    break                   #  PARSER-NEXT:    break
    continue                #  PARSER-NEXT:    continue
print(x)                    #  PARSER-NEXT:print(x)
