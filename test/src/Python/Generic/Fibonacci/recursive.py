# RUN: %S/../../test-parser.sh %s
# RUN: %S/../../test.sh %s


def fib(n):                             # PARSER-LABEL:def fib(n):
    if n == 0:                          #  PARSER-NEXT:    if (n == 0i):
        return 0                        #  PARSER-NEXT:        return 0i
    elif n == 1:                        #  PARSER-NEXT:    elif (n == 1i):
        return 1                        #  PARSER-NEXT:        return 1i
    else:                               #  PARSER-NEXT:    else:
        return fib(n - 1) + fib(n - 2)  #  PARSER-NEXT:        return (fib((n - 1i)) + fib((n - 2i)))


# PARSER-NEXT:print("Start")
print("Start")      # CHECK-LABEL: Start
print(fib(0))       #  CHECK-NEXT: 0
print(fib(1))       #  CHECK-NEXT: 1
print(fib(2))       #  CHECK-NEXT: 1
print(fib(3))       #  CHECK-NEXT: 2
print(fib(4))       #  CHECK-NEXT: 3
print(fib(5))       #  CHECK-NEXT: 5
print(fib(6))       #  CHECK-NEXT: 8
print("End")        # CHECK-LABEL: End
