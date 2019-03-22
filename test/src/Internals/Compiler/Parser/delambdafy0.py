# RUN: test-parser.sh %s delambdafy
# RUN: test-output.sh %s

print("Start")
# PARSER-LABEL:print("Start")
# OUTPUT-LABEL: Start

first = lambda: 1

# PARSER-NEXT:def __lambda__0():
# PARSER-NEXT:    return 1i
# PARSER-NEXT:first = __lambda__0

print(str(first()))
# PARSER-NEXT:print(str(first()))
# OUTPUT-NEXT: 1

print("Middle")
# PARSER-NEXT:print("Middle")
# OUTPUT-NEXT: Middle

def func(y):
    second = 1 + (lambda x: x + 2)(y)
    return second

# PARSER-NEXT:def func(y):
# PARSER-NEXT:    def __lambda__1(x):
# PARSER-NEXT:        return (x + 2i)
# PARSER-NEXT:    second = (1i + __lambda__1(y))
# PARSER-NEXT:    return second

print(str(func(10)))
# PARSER-NEXT:print(str(func(10i)))
# OUTPUT-NEXT: 13

print("Third")
# PARSER-NEXT:print("Third")
# OUTPUT-NEXT: Third

def cnuf(y):
    third = (lambda x: x + 2)(y) + (lambda z: z + 3)(y) + first()
    return third

# PARSER-NEXT:def cnuf(y):
# PARSER-NEXT:    def __lambda__2(x):
# PARSER-NEXT:        return (x + 2i)
# PARSER-NEXT:    def __lambda__3(z):
# PARSER-NEXT:        return (z + 3i)
# PARSER-NEXT:    third = ((__lambda__2(y) + __lambda__3(y)) + first())
# PARSER-NEXT:    return third

print(str(cnuf(10)))
# PARSER-NEXT:print(str(cnuf(10i)))
# OUTPUT-NEXT: 26

def nucf(y):
    tup = (lambda x: x + 1, lambda x: x + 2, lambda x: x + 3)

# PARSER-NEXT:def nucf(y):
# PARSER-NEXT:    def __lambda__4(x):
# PARSER-NEXT:        return (x + 1i)
# PARSER-NEXT:    def __lambda__5(z):
# PARSER-NEXT:        return (z + 2i)
# PARSER-NEXT:    def __lambda__6(z):
# PARSER-NEXT:        return (z + 3i)
# PARSER-NEXT:    tup = (__lambda__4, __lambda__5, __lambda__6)

print("End")
# PARSER-NEXT:print("End")
# OUTPUT-NEXT: End
