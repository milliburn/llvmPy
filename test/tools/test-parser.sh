#!/usr/bin/env sh
set -e
src=$1
t1=$(mktemp)
llvmPy -X phase=parser $src > $t1
cat -n $t1 >&2
cat $t1 | FileCheck $src --match-full-lines --strict-whitespace --check-prefix=PARSER
