# RUN: test-output.sh %s

x = ()
y = 1,
z = 1, 2

print('Start')      #      OUTPUT: Start
print(x)            # OUTPUT-NEXT: ()
print(y)            # OUTPUT-NEXT: (1,)
print(z)            # OUTPUT-NEXT: (1, 2)

print(len(x))       # OUTPUT-NEXT: 0
print(len(y))       # OUTPUT-NEXT: 1
print(len(z))       # OUTPUT-NEXT: 2

print(x < y)        # OUTPUT-NEXT: True
