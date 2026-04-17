#!/bin/sh

cat './bins' | cpio -H newc -o >'kernelshit1337/linux/init.cpio'

cd kernelshit1337/linux/

make isoimage FDARGS="initrd=/init.cpio" FDINITRD=init.cpio
