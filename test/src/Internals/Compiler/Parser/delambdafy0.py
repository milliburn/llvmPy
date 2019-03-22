# RUN: test-parser.sh %s delambdafy

print("Start")

# PARSER-LABEL:print("Start")

first = lambda: 1

# PARSER-NEXT:def __lambda__0():
# PARSER-NEXT:    return 1i
# PARSER-NEXT:first = __lambda__0

print("Middle")
# PARSER-NEXT:print("Middle")

def func(y):
    second = 1 + (lambda x: x + 2)(y)

# PARSER-NEXT:def func(y):
# PARSER-NEXT:    def __lambda__1(x):
# PARSER-NEXT:        return (x + 2i)
# PARSER-NEXT:    second = (1i + __lambda__1(y))

print("End")

# PARSER-NEXT:print("End")
