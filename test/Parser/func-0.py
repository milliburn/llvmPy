# RUN: %S/test.sh %s


def func():                 # CHECK-LABEL:def func():
    x = 1 + 2               #  CHECK-NEXT:    x = (1i + 2i)
    return x                #  CHECK-NEXT:    return x


def func():                 # CHECK-LABEL:def func():
    return 1 + 2 * 3 * 4    #  CHECK-NEXT:    return (1i + ((2i * 3i) * 4i))
