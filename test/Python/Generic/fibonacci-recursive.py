# RUN: %S/../test.sh %s


def fib(n):
    if n == 0:
        return 0
    elif n == 1:
        return 1
    else:
        return fib(n - 1) + fib(n - 2)

print("Start")      # CHECK-LABEL: Start
print(fib(0))       #  CHECK-NEXT: 0
print(fib(1))       #  CHECK-NEXT: 1
print(fib(2))       #  CHECK-NEXT: 1
print(fib(3))       #  CHECK-NEXT: 2
print(fib(4))       #  CHECK-NEXT: 3
print(fib(5))       #  CHECK-NEXT: 5
print(fib(6))       #  CHECK-NEXT: 8
print("End")        # CHECK-LABEL: End
