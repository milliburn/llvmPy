#!/usr/bin/env sh
set -e

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ ! -e "$CURRENT_DIR/.venv/bin/activate" ]; then
    if [ -x "$(command -v python3)" ]; then
        python3 -m venv "$CURRENT_DIR/.venv"
    else
        echo "Requires Python 3"
    fi
fi

source "$CURRENT_DIR/.venv/bin/activate"
