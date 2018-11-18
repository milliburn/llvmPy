# RUN: %S/../test.sh %s

s1 = ""
s2 = "a"
s3 = "test"

print("start")                      # CHECK-LABEL: start
print("a" + "test")                 # CHECK-NEXT: atest
print("a" + "test" + s2)            # CHECK-NEXT: atesta
print("a" == "a")                   # CHECK-NEXT: True
print(s1 == s1)                     # CHECK-NEXT: True
print("a" == "test")                # CHECK-NEXT: False
print(len(""))                      # CHECK-NEXT: 0
print(len("test"))                  # CHECK-NEXT: 4
