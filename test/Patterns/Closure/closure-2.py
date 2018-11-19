# RUN: test-output.sh %s



def adder(x):
    y = 2

    def inner(z):
        return x + y + z

    return inner

a1 = adder(10)
a2 = adder(20)

print("Start")      # OUTPUT-LABEL: Start
print(a1(0))        # OUTPUT-NEXT: 12
print(a2(0))        # OUTPUT-NEXT: 22
print(a1(0))        # OUTPUT-NEXT: 12
print(a2(0))        # OUTPUT-NEXT: 22
print(a1(1))        # OUTPUT-NEXT: 13
print(a2(1))        # OUTPUT-NEXT: 23
print("End")        # OUTPUT-LABEL: End
