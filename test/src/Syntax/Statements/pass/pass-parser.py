# RUN: test-parser.sh %s


test_case = 1                   # PARSER-LABEL:test_case = 1i
def func():                     #  PARSER-NEXT:def func():
    pass                        #  PARSER-NEXT:    pass
