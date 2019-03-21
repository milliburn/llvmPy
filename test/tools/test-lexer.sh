#!/usr/bin/env sh
set -e
src=$1
t1=$(mktemp)
llvmPy -X lexer $src > $t1
cat -n $t1 >&2
cat $t1 | FileCheck $src --match-full-lines --check-prefix=LEXER
