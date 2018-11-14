# RUN: %S/../test.sh %s


def func(x):            # CHECK-LABEL:def func(x):
    x = x + 1           #  CHECK-NEXT:    x = (x + 1i)
    z = 9 + 1           #  CHECK-NEXT:    z = (9i + 1i)
    return x + z        #  CHECK-NEXT:    return (x + z)
