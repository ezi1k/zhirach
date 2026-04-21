#!/bin/sh
export govno="$(pwd)"

cd fs
rm $govno/kernelshit1337/init.cpio
find . | cpio -H newc -o >"$govno/kernelshit1337/init.cpio"

cd $govno/kernelshit1337/linux/

make isoimage FDARGS="initrd=/init.cpio" FDINITRD="$govno/kernelshit1337/init.cpio" -j$(nproc)
echo "$govno"
