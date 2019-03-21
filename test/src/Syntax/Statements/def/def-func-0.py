# RUN: test-parser.sh %s


def func():                 # PARSER-LABEL:def func():
    x = 1 + 2               #  PARSER-NEXT:    x = (1i + 2i)
    return x                #  PARSER-NEXT:    return x


def func():                 # PARSER-LABEL:def func():
    return 1 + 2 * 3 * 4    #  PARSER-NEXT:    return (1i + ((2i * 3i) * 4i))
