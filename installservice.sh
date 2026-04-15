#!/bin/sh

rm /etc/zinit/dhcpcd.sh
unlink /etc/init.z/dhcpcd.sh
cp zservice/* /etc/zinit/
ln -s /etc/zinit/dhcpcd.sh /etc/init.z/dhcpcd.sh
chmod +x /etc/zinit/*
