#!/bin/sh
mkdir bin
mkdir bin/static

gcc zinit/main.c -o ./bin/init &
gcc zshell/main.c -o ./bin/zshell &

gcc zinit/main.c -static -o ./bin/static/init &
gcc zshell/main.c -static -o ./bin/static/zshell &

ldd bin/*
ldd bin/static/*
