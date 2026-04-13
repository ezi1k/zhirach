#!/bin/sh
mkdir bin

gcc zinit/main.c -j$(nproc) -o ./bin/init
gcc zshell/main.c -j$(nproc) -o ./bin/zshell
