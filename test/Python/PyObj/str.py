# RUN: %S/../test.sh %s

print("start")                      # CHECK-LABEL: start
print("a " + "test")                # CHECK-NEXT: a test
print("a " + "test" + " b")         # CHECK-NEXT: a test b
print(len(""))                      # CHECK-NEXT: 0
print(len("test"))                  # CHECK-NEXT: 4
