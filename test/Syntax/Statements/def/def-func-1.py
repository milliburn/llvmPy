# RUN: test-parser.sh %s


def func(x):            # PARSER-LABEL:def func(x):
    x = x + 1           #  PARSER-NEXT:    x = (x + 1i)
    z = 9 + 1           #  PARSER-NEXT:    z = (9i + 1i)
    return x + z        #  PARSER-NEXT:    return (x + z)
