# RUN: test-x86.sh %s

# Notice triple underscore at the beginning.
# X86: .globl ___body__
# X86-LABEL: ___body__:
