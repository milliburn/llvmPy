# RUN: %S/../test.sh %s

# CHECK-LABEL: while:
# CHECK: br i1 %{{[0-9]+}}, label %loop, label %endwhile
while True:
    # CHECK-LABEL: loop:
    if True:
        # CHECK-LABEL: then:
        # CHECK: br label %endwhile
        break
    # CHECK-LABEL: endif:
    # CHECK: br label %while
# CHECK-LABEL: endwhile:
