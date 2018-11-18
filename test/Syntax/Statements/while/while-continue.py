# RUN: test-ir.sh %s

# IR-LABEL: while:
# IR: br i1 %{{[0-9]+}}, label %loop, label %endwhile
while True:
    # IR-LABEL: loop:
    if True:
        # IR-LABEL: then:
        # IR: br label %while
        continue
    # IR-LABEL: endif:
    # IR: br label %while
# IR-LABEL: endwhile:
