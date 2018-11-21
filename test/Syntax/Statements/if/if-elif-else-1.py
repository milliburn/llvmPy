# RUN: test-ir.sh %s


# IR-LABEL: define %PyObj* @__body__

x = 0

# IR-LABEL: if.0:
# IR: @llvmPy_eq
# IR: @llvmPy_truthy
# IR: br i1 {{%[^ ]+}}, label %then.0, label %else.0
if x == 1:
    # IR-LABEL: then.0:
    x = 10
# IR-LABEL: else.0:
# IR-NEXT: br label %if.1
# IR-LABEL: if.1:
# IR: @llvmPy_ge
# IR: @llvmPy_truthy
# IR: br i1 {{%[^ ]+}}, label %then.1, label %else.1
elif x >= 2:
    # IR-LABEL: then.1:
    x = 11
# IR-LABEL: else.1:
# IR-NEXT: br label %if.2
# IR-LABEL: if.2:
# IR: @llvmPy_ne
# IR: @llvmPy_truthy
# IR: br i1 {{%[^ ]+}}, label %then.2, label %else.2
elif x != 3:
    # IR-LABEL: then.2:
    x = 12
# IR-LABEL: else.2:
else:
    x = 13

# IR-LABEL: endif.2:
# IR-LABEL: br label %endif.1

# IR-LABEL: endif.1:
# IR-LABEL: br label %endif.0

# IR-LABEL: endif.0:

print(x)
# IR: @llvmPy_print
# IR-NEXT: ret %PyObj* @llvmPy_None
