# RUN: test-output.sh %s

x = 1

def adder():
    y = 2

    def inner(z):
        return x + y + z

    return inner(10)

print("Start")      # OUTPUT-LABEL: Start
print(adder())      # OUTPUT-NEXT: 13
print("End")        # OUTPUT-LABEL: End
