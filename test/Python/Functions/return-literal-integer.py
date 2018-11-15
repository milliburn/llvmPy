# RUN: %S/../test.sh %s


def func():
    return 1


print(func())  # CHECK: 1
