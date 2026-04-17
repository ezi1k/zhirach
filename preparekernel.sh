#!/bin/sh

mkdir kernelshit1337
cd kernelshit1337

git clone https://github.com/torvalds/linux --depth 1

cd linux

make defconfig

echo "skachack gta5$(nproc)"
sleep 2
make -j$(nproc)
