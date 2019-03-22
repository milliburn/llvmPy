# RUN: test-parser.sh %s

tup = (lambda x: x + 1, lambda x: x + 2, lambda x: x + 3)
# PARSER-NEXT:tup = ((lambda x: (x + 1i)), (lambda x: (x + 2i)), (lambda x: (x + 3i)))
