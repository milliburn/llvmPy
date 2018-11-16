# RUN: %S/../test.sh %s


def fib(N):
    if N < 2:
        return N

    i = 1
    tmp = 0
    result = 1

    while i < N:
        tmp1 = result
        result = result + tmp
        tmp = tmp1
        i += 1

    return result

print("Start")      # CHECK-LABEL: Start
print(fib(0))       #  CHECK-NEXT: 0
print(fib(1))       #  CHECK-NEXT: 1
print(fib(2))       #  CHECK-NEXT: 1
print(fib(3))       #  CHECK-NEXT: 2
print(fib(4))       #  CHECK-NEXT: 3
print(fib(5))       #  CHECK-NEXT: 5
print(fib(6))       #  CHECK-NEXT: 8
print("End")        # CHECK-LABEL: End
