#!/bin/sh
export hugedick="$(pwd)"

cd fs
find . | cpio -H newc -o >"$hugedick/bin/cpio/init.cpio"

cd $hugedick/kernelshit1337/linux/

make isoimage FDARGS="initrd=/init.cpio" FDINITRD="$hugedick/kernelshit1337/init.cpio"
