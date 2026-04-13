#!/bin/sh
mkdir ./bin

gcc ./zinit/main.c -o -j$(nproc) ./bin/init &
gcc ./zshell/main.c -o -j$(nproc) ./bin/zshell &
