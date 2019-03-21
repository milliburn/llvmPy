#!/usr/bin/env sh
set -e
src=$1
t1=$(mktemp)
llvmPy -X ir $src | llc -march=x86 > $t1
cat -n $t1 >&2
cat $t1 | FileCheck $src --check-prefix=X86

