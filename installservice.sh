#!/bin/sh
mkdir fs/etc/init.z fs/etc/zinit

rm fs/etc/zinit/dhcpcd.sh
unlink fs/etc/init.z/dhcpcd.sh
cp zservice/* fs/etc/zinit/
ln -s fs/etc/zinit/dhcpcd.sh fs/etc/init.z/dhcpcd.sh
chmod +x fs/etc/zinit/*
