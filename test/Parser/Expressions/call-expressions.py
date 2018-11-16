# RUN: %S/../test.sh %s

f(x + 1)                # CHECK:f((x + 1i))
f(x + 2 + 3)            # CHECK-NEXT:f(((x + 2i) + 3i))
f(x + 1) + f(x + 2)     # CHECK-NEXT:(f((x + 1i)) + f((x + 2i))
1 + f(x + 1)            # CHECK-NEXT:(1i + f((x + 1i)))
f(x + 1) + 1            # CHECK-NEXT:(f((x + 1i)) + 1i)

# Pretend (f + 1) returns a callable.
(f + 1)(x + 1)          # CHECK-NEXT:(f + 1i)((x + 1i))
