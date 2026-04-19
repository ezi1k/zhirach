#!/bin/sh
export hugedick="$(pwd)"

mkdir kernelshit1337
cd kernelshit1337

git clone https://github.com/torvalds/linux --depth 1

cd linux
git pull
cp $hugedick/cfg/kernel/.config .

echo "skachack gta5$(nproc)"
sleep 2
make -j$(nproc)
