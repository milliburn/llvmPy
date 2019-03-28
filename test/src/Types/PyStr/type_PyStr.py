# RUN: test-output.sh %s

# Mirrors type_PyStr.cc

s1 = ""
s2 = "a"
s3 = "test"
s4 = "4"
s5 = "-5"
s6 = "0"
s7 = "True"
s8 = "False"

print("start")  # OUTPUT-LABEL: start

# py__len__()
print(len(s1))  # OUTPUT-NEXT: 0
print(len(s2))  # OUTPUT-NEXT: 1
print(len(s3))  # OUTPUT-NEXT: 4

# py__str__()
print(str(s1))  # OUTPUT-EMPTY:
print(str(s2))  # OUTPUT-NEXT: a
print(str(s3))  # OUTPUT-NEXT: test

# py__int__()
print(int(s4))  # OUTPUT-NEXT: 4
print(int(s5))  # OUTPUT-NEXT: 5

# py__bool__()
print(bool(s1))  # OUTPUT-NEXT: False
print(bool(s2))  # OUTPUT-NEXT: True
print(bool(s7))  # OUTPUT-NEXT: True
print(bool(s8))  # OUTPUT-NEXT: True

# py__eq__()
print(s1 == s1)  # OUTPUT-NEXT: True
print(s1 == s2)  # OUTPUT-NEXT: False
print(s2 == s1)  # OUTPUT-NEXT: False
print(s2 == s2)  # OUTPUT-NEXT: True
print("a" == "a")  # OUTPUT-NEXT: True

# py__ne__()
print(s1 != s1)  # OUTPUT-NEXT: False
print(s1 != s2)  # OUTPUT-NEXT: True

# py__add__()
print(s1 + s1)  # OUTPUT-EMPTY:
print(s1 + s2)  # OUTPUT-NEXT: a
print(s2 + s2)  # OUTPUT-NEXT: aa
print(s1 + s2 + s3 + s4)  # OUTPUT-NEXT: atest4

# py__getitem__()
print(s3[0])  # OUTPUT-NEXT: t
print(s3[1])  # OUTPUT-NEXT: e

print("end")  # OUTPUT-LABEL: end
