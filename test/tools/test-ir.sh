#!/usr/bin/env sh
set -e
src=$1
t1=$(mktemp)
llvmPy --ir $src > $t1
cat -n $t1 >&2
llvm-as < $t1 | llvm-dis | FileCheck $src --check-prefix=IR
