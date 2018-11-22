# RUN: test-output.sh %s

name = "test"
func = name.upper
print("Start")      # OUTPUT-LABEL: Start
print(name)         # OUTPUT-NEXT: test
print(func())       # OUTPUT-NEXT: TEST
print(func)         # OUTPUT-NEXT: <PyFunc LM 0x{{[0-9A-F]+}}>
print(name)         # OUTPUT-NEXT: test
