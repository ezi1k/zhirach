#!/bin/sh
mkdir bin

gcc zinit/main.c -o ./bin/init &
gcc zshell/main.c -o ./bin/zshell &
