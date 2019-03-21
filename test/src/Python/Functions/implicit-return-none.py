# RUN: %S/../test.sh %s


def func():
    x = 1


print(func())  # CHECK: None
