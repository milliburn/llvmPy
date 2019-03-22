# RUN: test-parser.sh %s

y = 1
d = lambda x: x + 2
e = (lambda x: x + 2)
f = (lambda x: x + 2)(y)

# PARSER:y = 1i
# PARSER-NEXT:d = (lambda x: (x + 2i))
# PARSER-NEXT:e = (lambda x: (x + 2i))
# PARSER-NEXT:f = (lambda x: (x + 2i))(y)
