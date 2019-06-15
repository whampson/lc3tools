#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" > /dev/null 2>&1 && pwd)"

open -a Terminal $SCRIPT_DIR/../../build/bin/lc3emu
ping -c 2 127.0.0.1         # delay to allow Terminal to launch
$SCRIPT_DIR/../test/dbg.sh
