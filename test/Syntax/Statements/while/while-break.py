# RUN: test-ir.sh %s

# IR-LABEL: while.0:
# IR: br i1 %{{[0-9]+}}, label %loop.0, label %endwhile.0
while True:
    # IR-LABEL: loop.0:
    if True:
        # IR-LABEL: then.0:
        # IR: br label %endwhile.0
        break
    # IR-LABEL: endif.0:
    # IR: br label %while.0
# IR-LABEL: endwhile.0:
