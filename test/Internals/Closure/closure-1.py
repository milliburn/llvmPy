# RUN: test-output.sh %s

x = 1

def adder():
    y = 2

    def inner(z):
        return x + y + z

    return inner

f = adder()

print("Start")      # OUTPUT-LABEL: Start
print(f(0))         # OUTPUT-NEXT: 3
print(f(10))        # OUTPUT-NEXT: 13
print(f(20))        # OUTPUT-NEXT: 23
print("End")        # OUTPUT-LABEL: End
