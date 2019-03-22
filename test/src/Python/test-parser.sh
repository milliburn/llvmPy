#!/usr/bin/env sh
set -e
src=$1
t1=$(mktemp)
llvmPy -X pass=parser $src > $t1
cat -n $t1 >&2
cat $t1 | FileCheck $src --check-prefix=PARSER --match-full-lines --strict-whitespace

