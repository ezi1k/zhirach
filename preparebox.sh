#!/bin/sh
export govno="$(pwd)"

cd $govno/kernelshit1337/

git clone --depth 1 https://github.com/landley/toybox
cd toybox
git pull
cp $govno/cfg/toybox/.config .

make -j$(nproc)
PREFIX=$govno/fs make install
