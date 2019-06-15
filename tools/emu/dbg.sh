#!/bin/bash

emu_pid=$(ps -C | awk -F ' ' '{ if ($4 ~ /lc3emu/) print $1; }')
echo $emu_pid
sudo gdb -p $emu_pid
