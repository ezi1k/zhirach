#!/bin/sh

rm /etc/zinit/dhcpcd.sh
unlink /etc/init.z/dhcpcd.sh
cp zservice/* /etc/zinit/
ln -s /etc/init.z/dhcpcd.sh
chmod +x /etc/zinit/*
