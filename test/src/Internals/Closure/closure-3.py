# RUN: test-output.sh %s

x = 1

def adder():
    y = 2

    def a(x):
        return x * 2

    def b(y):
        return y + 2

    # The arguments are intentionally swapped by name.
    return a(y) + b(x)

print("Start")      # OUTPUT-LABEL: Start
print(adder())      # OUTPUT-NEXT: 7
print("End")        # OUTPUT-LABEL: End
