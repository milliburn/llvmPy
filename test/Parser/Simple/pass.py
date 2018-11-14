# RUN: %S/../test.sh %s


test_case = 1                   # CHECK-LABEL:test_case = 1i
def func():                     #  CHECK-NEXT:def func():
    pass                        #  CHECK-NEXT:    pass
