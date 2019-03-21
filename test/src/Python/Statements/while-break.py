# RUN: %S/../test.sh %s

x = 1
print("Start")
while True:
    x = x + 1
    if x == 5:
        print("Break")
        break
    print("Continue")
print("End")

# CHECK-LABEL: Start
# CHECK-NEXT: Continue
# CHECK-NEXT: Continue
# CHECK-NEXT: Continue
# CHECK-NEXT: Break
# CHECK-NEXT: End

